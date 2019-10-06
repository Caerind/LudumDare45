#include "GameState.hpp"

#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>

#include "EntityPrefab.hpp"
#include "EditorComponents.hpp"
#include "Components.hpp"
#include "GameSingleton.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
{
	GameSingleton::clear();

	GameSingleton::loadResourcesGame();
	GameSingleton::loadAnimations();

	//getApplication().getTextures().create("atmog", en::TextureLoader::loadFromFile("Assets/Textures/atmog.png"));
	//asteroidTextureId = getApplication().getTextures().create("asteroids", en::TextureLoader::loadFromFile("Assets/Textures/asteroids.png"));
	//fontId = getApplication().getFonts().create("font", en::FontLoader::loadFromFile("Assets/Fonts/ErasBoldITC.ttf"));

#ifdef ENLIVE_ENABLE_IMGUI
	GameSingleton::currentEntity = entt::null;
	EditorComponents::registerToEditor(GameSingleton::worldEditor);
#endif

	// mCursor
	getApplication().getWindow().setCursor(en::Window::Cursor::None);
	getApplication().getTextures().create("cursor", en::TextureLoader::loadFromFile("Assets/Textures/cursor.png"));
	mCursor.setTexture(getApplication().getTextures().get("cursor"));
	mCursor.setTextureRect(sf::IntRect(0, 0, 32, 32));
	mCursor.setOrigin(16, 16);
	mCursor.setScale(0.3f, 0.3f);
	mCursorTime = en::Time::Zero;

	// Audio
	getApplication().getAudio().setGlobalVolume(3.0f);

	// Music
	getApplication().getAudio().createMusic("mainTheme", "Assets/Musics/MainTheme.ogg");
	getApplication().getAudio().playMusic("mainTheme");

	// Tileset & Map
	GameSingleton::mTileset.setFirstGid(1);
	GameSingleton::mTileset.setTileSize(en::Vector2i(16, 16));
	GameSingleton::mTileset.setTileCount(160);
	GameSingleton::mTileset.setColumns(8);
	GameSingleton::mTileset.setImageSource("Assets/Textures/tileset.png");
	GameSingleton::mMap.load(1, en::Vector2f(408, 312), &GameSingleton::mTileset, en::Vector2i(16, 16));

	// View
	GameSingleton::mView.setCenter(GameSingleton::mMap.getSpawnPoint());
	GameSingleton::mView.setSize(getApplication().getWindow().getMainView().getSize() * 0.125f);

	EntityPrefab::createPlayer(GameSingleton::world, GameSingleton::playerEntity);
	GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).setPosition(en::toSF(GameSingleton::mMap.getSpawnPoint()));

	EntityPrefab::createNothing(GameSingleton::world, GameSingleton::nothingEntity);
	GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity).setPosition(en::toSF(GameSingleton::mMap.getSpawnPoint()));

	LogInfo(en::LogChannel::System, 9, "Loading game");
}

bool GameState::handleEvent(const sf::Event& event)
{
	ENLIVE_PROFILE_FUNCTION();

	if (event.type == sf::Event::Closed)
	{
		getApplication().stop();
	}

#ifdef ENLIVE_DEBUG
	devEvent(event);
#endif

	bool clickHandled = false;
	const en::Vector2f mPos = getApplication().getWindow().getCursorPositionView(GameSingleton::mView);
	const en::Vector2f cPos = getAdjustedCursorPos();

	if (!clickHandled && event.type == sf::Event::MouseButtonPressed)
	{
		if (GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::Throw);
				GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping = false;
				GameSingleton::world.get<en::RenderableComponent>(GameSingleton::nothingEntity).z = 0;
				GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).chopped = false;

				en::Vector2f d = getAdjustedCursorPos() - (en::toEN(GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition()) + en::Vector2f(0, -8));
				if (d.getSquaredLength() > 0.1f)
				{
					GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity).velocity = d.normalized() * 2.0f;
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::seconds(0.5f);
				}
				else
				{
					GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity).velocity = en::Vector2f::zero;
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::Time::Zero;
					GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::KO);
					GameSingleton::world.get<NothingComponent>(GameSingleton::playerEntity).koTimer = en::seconds(0.2f);
				}
			}
		}
		else
		{
			en::U32 action = 0;
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::HitIdle);
				action = 0;
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				action = 1;
			}

			auto view = GameSingleton::world.view<HumanComponent, en::PositionComponent>();
			for (auto entity : view)
			{
				if (entity == GameSingleton::playerEntity)
					continue;

				auto& position = view.get<en::PositionComponent>(entity);
				auto& human = view.get<HumanComponent>(entity);
				sf::FloatRect aabb = position.getTransform().transformRect(human.body.getGlobalBounds());

				if (aabb.contains(en::toSF(cPos)))
				{
					clickHandled = true;

					if (action == 0)
					{
						if (entity == GameSingleton::nothingEntity)
						{
							if (!GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isKO())
							{
								GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits++;
								if (GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits >= 5)
								{
									human.playAnimLoop(HumanComponent::Animation::KO);
									GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits = 0;
									GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::seconds(5.f);
								}
								else
								{
									human.playAnimOnce(HumanComponent::Animation::Hitted);
								}
							}
						}
						else
						{
							human.playAnimOnce(HumanComponent::Animation::Hitted);
						}
					}
					if (action == 1 && entity == GameSingleton::nothingEntity && GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isKO())
					{
						GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::Chop);
						human.playAnimLoop(HumanComponent::Animation::Chopped);
						GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping = true;
						GameSingleton::world.get<en::RenderableComponent>(GameSingleton::nothingEntity).z = 1;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).chopped = true;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits = 0;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::Time::Zero;
					}
					break;
				}
			}
		}
	}
	if (!clickHandled && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle)
	{
		GameSingleton::playClick();
		EntityPrefab::createAI(GameSingleton::world, mPos.x, mPos.y);
		
		clickHandled = true;
	}

	return false;
}

bool GameState::update(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

#ifdef ENLIVE_DEBUG
	devUpdate(dt);
#endif

	auto& playerPlayerComponent = GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity);
	auto& playerHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity);
	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& playerVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::playerEntity);

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity);
	auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);

	// AI components

	velocities();
	directions();

	// Player animation hacks
	if (playerHumanComponent.currentAnimation == HumanComponent::Animation::ChoppingIdle || playerHumanComponent.currentAnimation == HumanComponent::Animation::ChoppingWalking)
	{
		if (playerHumanComponent.walking && playerHumanComponent.currentAnimation == HumanComponent::Animation::ChoppingIdle)
		{
			playerHumanComponent.playAnimLoop(HumanComponent::Animation::ChoppingWalking);
		}
		else if (!playerHumanComponent.walking && playerHumanComponent.currentAnimation == HumanComponent::Animation::ChoppingWalking)
		{
			playerHumanComponent.playAnimLoop(HumanComponent::Animation::ChoppingIdle);
		}
	}
	if (playerHumanComponent.currentAnimation == HumanComponent::Animation::HitIdle || playerHumanComponent.currentAnimation == HumanComponent::Animation::HitWalking)
	{
		if (playerHumanComponent.walking && playerHumanComponent.currentAnimation == HumanComponent::Animation::HitIdle)
		{
			playerHumanComponent.currentAnimation = HumanComponent::Animation::HitWalking;
		}
		else if (!playerHumanComponent.walking && playerHumanComponent.currentAnimation == HumanComponent::Animation::HitWalking)
		{
			playerHumanComponent.currentAnimation = HumanComponent::Animation::HitIdle;
		}
	}

	// Nothing
	if (nothingNothingComponent.isKO())
	{
		nothingNothingComponent.koTimer -= dt;
		if (nothingNothingComponent.koTimer < en::Time::Zero)
		{
			nothingHumanComponent.playAnimLoop(HumanComponent::Animation::Idle);
			nothingNothingComponent.koTimer = en::Time::Zero;
		}
		nothingNothingComponent.hits = 0;
	}
	if (nothingNothingComponent.isThrowing())
	{
		nothingNothingComponent.throwTimer -= dt;
		if (nothingNothingComponent.throwTimer < en::Time::Zero)
		{
			nothingHumanComponent.playAnimOnce(HumanComponent::Animation::KO);
			nothingNothingComponent.koTimer = en::seconds(0.2f);
			nothingVelocityComponent.velocity = en::Vector2f::zero;
		}
	}

	// Animation
	auto viewAnim = GameSingleton::world.view<HumanComponent>();
	for (auto entity : viewAnim)
	{
		auto& human = viewAnim.get(entity);
		human.frameTime += dt;

		if (human.currentAnimation == HumanComponent::Animation::Idle || human.currentAnimation == HumanComponent::Animation::Walking)
		{
			if (human.walking && human.currentAnimation == HumanComponent::Animation::Idle)
			{
				human.playAnimLoop(HumanComponent::Animation::Walking);
			}
			else if (!human.walking && human.currentAnimation == HumanComponent::Animation::Walking)
			{
				human.playAnimLoop(HumanComponent::Animation::Idle);
			}
		}

		if (human.frameTime >= GameSingleton::mAnimations[human.getAnimIdx()].getFrame(human.currentFrame).duration)
		{
			human.frameTime = en::Time::Zero;

			human.currentFrame++;
			if (human.currentFrame >= GameSingleton::mAnimations[human.getAnimIdx()].getFrameCount())
			{
				human.currentFrame = 0;

				if (entity == GameSingleton::playerEntity)
				{
					if (human.currentAnimation == HumanComponent::Animation::Chop)
					{
						human.playAnimLoop(HumanComponent::Animation::ChoppingIdle);
					}
				}

				if (!human.animLoop)
				{
					human.playAnimLoop(HumanComponent::Animation::Idle);
				}
			}

			human.body.setTextureRect(en::toSF(GameSingleton::mAnimations[human.getAnimIdx()].getFrame(human.currentFrame).rect));
		}
	}

	auto viewAnimProps = GameSingleton::world.view<PropsComponent>();
	for (auto entity : viewAnimProps)
	{
		auto& props = viewAnimProps.get(entity);

		if (props.destructed)
		{
			props.destructTimeAnimation += dt;
			if (props.destructTimeAnimation >= PropsComponent::timePerFrame)
			{
				props.destructTimeAnimation = en::Time::Zero;
				props.destructCurrentFrame++;
				if (props.destructCurrentFrame < props.destructFrames)
				{
					sf::IntRect rect = props.sprite.getTextureRect();
					rect.left += 16;
					props.sprite.setTextureRect(rect);
				}
				else
				{
					GameSingleton::world.destroy(entity);
				}
			}
		}
	}

	movements(dt);

	// Cursor/View animation
	const en::Vector2f cPos = getAdjustedCursorPos();
	const en::Vector2f pPos = en::toEN(GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition()) + en::Vector2f(0, -8);
	const en::Vector2f lPos = en::Vector2f::lerp(cPos, pPos, 0.6f);
	GameSingleton::mView.setCenter(lPos);
	mCursor.setPosition(en::toSF(cPos));
	mCursorTime += dt;
	if (mCursorTime.asSeconds() > 0.1f)
	{
		mCursorTime = en::Time::Zero;
		mCursorFrame++;
		if (mCursorFrame >= 6)
		{
			mCursorFrame = 0;
		}
		mCursor.setTextureRect(sf::IntRect(32 * mCursorFrame, 0, 32, 32));
	}

	// Update rendering pos
	auto view = GameSingleton::world.view<en::PositionComponent, en::RenderableComponent>();
	for (auto entity : view)
	{
		auto& position = view.get<en::PositionComponent>(entity);
		auto& renderable = view.get<en::RenderableComponent>(entity);
		const sf::Vector2f p = position.getTransform().transformPoint(sf::Vector2f());
		renderable.x = p.x;
		renderable.y = p.y;
	}

	return false;
}

void GameState::render(sf::RenderTarget& target)
{
	ENLIVE_PROFILE_FUNCTION();

	getWindow().setView(GameSingleton::mView);

	GameSingleton::mMap.render(target);

	{
		ENLIVE_PROFILE_SCOPE(RenderSort);
		GameSingleton::world.sort<en::RenderableComponent>([](const auto& lhs, const auto& rhs)
		{
			if (lhs.z == rhs.z)
			{
				return lhs.y < rhs.y;
			}
			return lhs.z < rhs.z;
		});
	}

	{
		ENLIVE_PROFILE_SCOPE(RenderSystem);
		auto view = GameSingleton::world.view<en::RenderableComponent, en::PositionComponent>();
		for (auto entity : view)
		{
			const auto& renderable = view.get<en::RenderableComponent>(entity);
			const auto& position = view.get<en::PositionComponent>(entity);

			sf::RenderStates states;
			states.transform = position.getTransform();

#ifdef ENLIVE_DEBUG
			sf::CircleShape pos;
			pos.setRadius(1);
			pos.setOrigin(1, 1);
			if (entity == GameSingleton::playerEntity)
			{
				pos.setFillColor(sf::Color::Blue);
			}
			else if (entity == GameSingleton::nothingEntity)
			{
				pos.setFillColor(sf::Color::Green);
			}
			else
			{
				pos.setFillColor(sf::Color::Red);
			}
			target.draw(pos, states);
#endif

			if (GameSingleton::world.has<HumanComponent>(entity))
			{
				target.draw(GameSingleton::world.get<HumanComponent>(entity).body, states);
			}
			if (GameSingleton::world.has<PropsComponent>(entity))
			{
				target.draw(GameSingleton::world.get<PropsComponent>(entity).sprite, states);
			}
		}
	}

	target.draw(mCursor);

	getWindow().applyMainView();
}

en::Window& GameState::getWindow()
{
	return getApplication().getWindow();
}

en::Vector2f GameState::getAdjustedCursorPos()
{
	const en::Vector2f mPos = getApplication().getWindow().getCursorPositionView(GameSingleton::mView);
	const en::Vector2f pPos = en::toEN(GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition()) + en::Vector2f(0, -8);
	en::Vector2f delta = mPos - pPos;
	if (delta.getSquaredLength() > 20 * 20)
	{
		delta.setLength(20);
	}
	else if (delta.getSquaredLength() < 0.01f)
	{
		delta = en::Vector2f(1, 1);
	}
	return pPos + delta;
}

bool GameState::isCollision(const en::Vector2f& point)
{
	return isCollisionSlow(point);
}

bool GameState::isCollisionSlow(const en::Vector2f& point)
{
	if (isCollisionFast(point))
	{
		return true;
	}
	/*
	const sf::Vector2f pos = en::toSF(point);
	auto view = GameSingleton::world.view<en::PositionComponent, PropsComponent>();
	for (auto entity : view)
	{
		auto& position = view.get<en::PositionComponent>(entity);
		auto& props = view.get<PropsComponent>(entity);
		if (position.getTransform().transformRect(props.sprite.getGlobalBounds()).contains(pos))
		{
			return true;
		}
	}
	*/
	return false;
}

bool GameState::isCollisionFast(const en::Vector2f& point)
{
	if (GameSingleton::mMap.collide(GameSingleton::mMap.worldToCoords(point)))
	{
		return true;
	}
	return false;
}

void GameState::velocities()
{
	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& playerVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::playerEntity);
	auto& playerPlayerComponent = GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity);
	auto& playerHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity);

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);
	auto& nothingHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity);

	// AI
	/*
	auto view = GameSingleton::world.view<VelocityComponent>();
	for (auto entity : view)
	{
		auto& velocity = view.get(entity);
		if (entity == GameSingleton::playerEntity || entity == GameSingleton::nothingEntity)
		{
			continue;
		}
	}
	*/

	// Player
	if (playerHumanComponent.currentAnimation != HumanComponent::Animation::Chop)
	{
		playerVelocityComponent.velocity = playerPlayerComponent.getVelocity();
	}
	else
	{
		playerVelocityComponent.velocity = playerPlayerComponent.getVelocity() * 0.1f;
	}

	// Nothing
	if (nothingNothingComponent.isThrowing())
	{
		// Don't modify it
	}
	else if (!nothingNothingComponent.isKO() && !nothingNothingComponent.chopped)
	{
		en::Vector2f delta = en::toEN(playerPositionComponent.getPosition() - nothingPositionComponent.getPosition());
		if (delta.getSquaredLength() > 16.f * 16.f)
		{
			nothingVelocityComponent.velocity = delta.normalized();
		}
		else
		{
			nothingVelocityComponent.velocity = en::Vector2f::zero;
		}
	}
	else if (nothingNothingComponent.chopped)
	{
		nothingVelocityComponent.velocity = playerVelocityComponent.velocity;
	}
	else if (nothingNothingComponent.isKO())
	{
		nothingVelocityComponent.velocity = en::Vector2f::zero;
	}
}

void GameState::directions()
{
	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& playerHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity);

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);

	auto view = GameSingleton::world.view<HumanComponent, VelocityComponent>();
	for (auto entity : view)
	{
		auto& human = view.get<HumanComponent>(entity);
		auto& velocity = view.get<VelocityComponent>(entity);
		human.walking = velocity.velocity.getSquaredLength() > 0.1f;

		if (entity == GameSingleton::playerEntity)
		{
			en::F32 angle = 0.0f;
			en::Vector2f deltaView = getAdjustedCursorPos() - (en::toEN(playerPositionComponent.getPosition()) + en::Vector2f(0, -8));
			if (deltaView.getSquaredLength() < 0.01f)
			{
				angle = 45;
			}
			else
			{
				angle = deltaView.getPolarAngle();
			}
			if (angle < -0.01f)
			{
				angle += 360.0f;
			}
			if (angle >= -0.01f && angle < 90.0f)
			{
				human.direction = HumanComponent::Direction::BasDroite;
			}
			else if (angle >= 90.0f && angle <= 180.01f)
			{
				human.direction = HumanComponent::Direction::BasGauche;
			}
			else if (angle >= 180.01f && angle < 270.0f)
			{
				human.direction = HumanComponent::Direction::HautGauche;
			}
			else
			{
				human.direction = HumanComponent::Direction::HautDroite;
			}
		}
		else if (entity == GameSingleton::nothingEntity)
		{
			if (nothingNothingComponent.chopped)
			{
				human.direction = playerHumanComponent.direction;
			}
			else if (nothingNothingComponent.isKO())
			{
				human.direction = HumanComponent::Direction::BasGauche;
			}
			else
			{
				if (nothingVelocityComponent.velocity.x != 0.0f || nothingVelocityComponent.velocity.y != 0.0f)
				{
					en::F32 angle = nothingVelocityComponent.velocity.getPolarAngle();
					if (angle < -0.01f)
					{
						angle += 360.0f;
					}
					if (angle >= -0.01f && angle < 90.0f)
					{
						human.direction = HumanComponent::Direction::BasDroite;
					}
					else if (angle >= 90.0f && angle <= 180.01f)
					{
						human.direction = HumanComponent::Direction::BasGauche;
					}
					else if (angle >= 180.01f && angle < 270.0f)
					{
						human.direction = HumanComponent::Direction::HautGauche;
					}
					else
					{
						human.direction = HumanComponent::Direction::HautDroite;
					}
				}
				else
				{
					// Stay as it is
				}
			}
		}
		else
		{
			// TODO : AI
		}
	}
}

void GameState::movements(en::Time dt)
{
	auto viewMvt = GameSingleton::world.view<en::PositionComponent, VelocityComponent>();
	for (auto entity : viewMvt)
	{
		auto& position = viewMvt.get<en::PositionComponent>(entity);
		auto& velocity = viewMvt.get<VelocityComponent>(entity);
		if (velocity.velocity.x != 0.0f || velocity.velocity.y != 0.0f)
		{
			const en::F32 pxPerSeconds = 50.0f;
			const en::F32 velConst = dt.asSeconds() * pxPerSeconds;
			const en::F32 velX = velConst * velocity.velocity.x;
			const en::F32 velY = velConst * velocity.velocity.y;
			const en::Vector2f pos = en::toEN(position.getPosition());
			const en::Vector2f newPos = pos + en::Vector2f(velX, velY);
			if (!isCollision(newPos))
			{
				position.setPosition(en::toSF(newPos));
			}
			else if (velocity.velocity.x != 0.0f && velocity.velocity.y != 0.0f)
			{
				if (entity == GameSingleton::nothingEntity && GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isThrowing())
				{
					velocity.velocity = en::Vector2f::zero;
					GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimLoop(HumanComponent::Animation::KO);
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::seconds(1.5f);
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::Time::Zero;

					auto viewDestr = GameSingleton::world.view<en::PositionComponent, PropsComponent>();
					for (auto entity : viewDestr)
					{
						auto& position = viewDestr.get<en::PositionComponent>(entity);
						auto& props = viewDestr.get<PropsComponent>(entity);
						
						en::Vector2f d = newPos - en::toEN(position.getPosition());
						if (d.getSquaredLength() < 30.f * 30.f)
						{
							props.destruct();
						}
					}
				}
				else
				{
					const en::F32 newX = ((velX >= 0.0f) ? 1.0f : -1.0f);
					const en::Vector2f newPosX = pos + en::Vector2f(velConst * ((velX >= 0.0f) ? 1.0f : -1.0f), 0.f);
					bool xTranslationPossible = !isCollision(newPosX);
					if (xTranslationPossible)
					{
						position.setPosition(en::toSF(newPosX));
					}
					else
					{
						const en::F32 newY = ((velY >= 0.0f) ? 1.0f : -1.0f);
						const en::Vector2f newPosY = pos + en::Vector2f(0.f, velConst * ((velY >= 0.0f) ? 1.0f : -1.0f));
						bool yTranslationPossible = !isCollision(newPosY);
						if (yTranslationPossible)
						{
							position.setPosition(en::toSF(newPosY));
						}
					}
				}
			}
		}
	}

	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);

	if (nothingNothingComponent.chopped)
	{
		nothingPositionComponent.setPosition(playerPositionComponent.getPosition() + sf::Vector2f(0, -8));
	}
}

#ifdef ENLIVE_DEBUG
void GameState::devEvent(const sf::Event& event)
{
	ENLIVE_PROFILE_FUNCTION();

	// Screenshot
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
	{
		getWindow().screenshot();
	}

	// Config
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2)
	{
		GameConfig::showWindow = !GameConfig::showWindow;
	}

	// Metrics
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3)
	{
		GameSingleton::showWindow = !GameSingleton::showWindow;
	}

#ifdef ENLIVE_ENABLE_IMGUI
	// Entities
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F4)
	{
		GameSingleton::worldEditor.show_window = !GameSingleton::worldEditor.show_window;
	}
#endif // ENLIVE_ENABLE_IMGUI

	/*
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F5)
	{
		static bool zoomed = true;
		if (zoomed)
		{
			getView().setZoom(10.f);
		}
		else
		{
			getView().setZoom(2.0f);
		}
		zoomed = !zoomed;
	}
	*/
}

void GameState::devUpdate(en::Time& dt)
{
	ENLIVE_PROFILE_FUNCTION();

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
	{
		dt *= 2.0f;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		dt *= 0.5f;
	}

	if (GameConfig::showWindow)
	{
		GameConfig::renderConfigWindow();
	}
	if (GameSingleton::showWindow)
	{
		GameSingleton::renderMetricsWindow();
	}

#ifdef ENLIVE_ENABLE_IMGUI
	if (GameSingleton::worldEditor.show_window)
	{
		if (ImGui::Begin("Entity list"))
		{
			GameSingleton::world.view<en::NameComponent>().each([](auto entity, auto& name)
			{
				if (ImGui::Button(name.c_str()))
				{
					GameSingleton::currentEntity = entity;
				}
			});
			ImGui::End();
		}
		GameSingleton::worldEditor.renderImGui(GameSingleton::world, GameSingleton::currentEntity);
	}
#endif
}
#endif // ENLIVE_DEBUG
