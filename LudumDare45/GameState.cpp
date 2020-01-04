#include "GameState.hpp"

#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>
#include <Enlivengine/Math/Random.hpp>

#include "EntityPrefab.hpp"
#include "EditorComponents.hpp"
#include "Components.hpp"
#include "GameSingleton.hpp"

#include "MenuState.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
	, mText(GameSingleton::application->getResourceManager().Get<en::Texture>("buble"), GameSingleton::mFont)
	, mCursorFrame(0)
{
	GameSingleton::clear();

	GameSingleton::mMap.load(1, en::Vector2f(408, 312), &GameSingleton::mTileset, en::Vector2i(16, 16));

#ifdef ENLIVE_ENABLE_IMGUI
	static bool editorRegisterDone = false;
	if (!editorRegisterDone)
	{
		EditorComponents::registerToEditor(GameSingleton::worldEditor);
		editorRegisterDone = true;
	}
#endif

	// mCursor
	mCursor.setTexture(getApplication().getResourceManager().Get<en::Texture>("cursor").Get());
	mCursor.setTextureRect(sf::IntRect(0, 0, 32, 32));
	mCursor.setOrigin(16, 16);
	mCursor.setScale(0.3f, 0.3f);
	mCursorTime = en::Time::Zero;

	// Tutorial
	if (GameSingleton::mFirstThrowNothingDone)
	{
		mText.handleEvent(NothingTextSystem::Event::FightBegin);
		tutorialState = 99;
	}
	else
	{
		mText.handleEvent(NothingTextSystem::Event::Tutorial1);
		tutorialState = 0;
	}

	// Audio
	if (GameSingleton::soundEnabled)
	{
		getApplication().getAudio().setGlobalVolume(50.0f);

		// Music
		getApplication().getAudio().createMusic("mainTheme", "Assets/Musics/MainTheme.ogg");
		getApplication().getAudio().playMusic("mainTheme");
	}

	// View
	GameSingleton::mView.setCenter(GameSingleton::mMap.getSpawnPoint());
	GameSingleton::mView.setSize(getApplication().getWindow().getMainView().getSize() * 0.125f);

	EntityPrefab::createPlayer(GameSingleton::world, GameSingleton::playerEntity);
	GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).setPosition(en::toSF(GameSingleton::mMap.getSpawnPoint()));

	EntityPrefab::createNothing(GameSingleton::world, GameSingleton::nothingEntity);
	GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity).setPosition(en::toSF(GameSingleton::mMap.getSpawnPoint()) + sf::Vector2f(-20, -10));

	mPieceGenerator.setSeed(45676567);
}

bool GameState::handleEvent(const sf::Event& event)
{
	ENLIVE_PROFILE_FUNCTION();

#ifdef ENLIVE_DEBUG
	devEvent(event);
#endif

	// Screenshot
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
	{
		getApplication().getScreenshotSystem().screenshot(getWindow().getHandle());
	}

	bool clickHandled = false;
	const en::Vector2f mPos = getApplication().getWindow().getCursorPositionView(GameSingleton::mView);
	const en::Vector2f cPos = getAdjustedCursorPos();
	const en::Vector2f pPos = en::toEN(GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition()) + en::Vector2f(0.0f, -8.0f);
	const en::Vector2f d = cPos - pPos;

	if (!clickHandled && event.type == sf::Event::MouseButtonPressed)
	{
		if (GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping)
		{
			if (event.mouseButton.button == sf::Mouse::Left)
			{
				if (!GameSingleton::mFirstThrowNothingDone)
				{
					mText.handleEvent(NothingTextSystem::Event::FightBegin);
					GameSingleton::mFirstThrowNothingDone = true;
					auto viewDoorEnable = GameSingleton::world.view<DoorComponent>();
					for (auto entity : viewDoorEnable)
					{
						auto& door = viewDoorEnable.get(entity);
						door.enabled = true;
					}
				}
				GameSingleton::playSound(GameSingleton::mThrowSound);
				GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::Throw);
				GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping = false;
				GameSingleton::world.get<en::RenderableComponent>(GameSingleton::nothingEntity).z = 0;
				GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).chopped = false;
				if (d.getSquaredLength() > 0.1f)
				{
					GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity).velocity = d.normalized() * 2.0f;
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::seconds(0.5f);
				}
				else
				{
					GameSingleton::playSound(GameSingleton::mKnockoutSound);
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
				GameSingleton::playSound(GameSingleton::mHitSound);
				GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::HitIdle);
				action = 0;

				if (tutorialState == 1)
				{
					tutorialState++;
				}
			}
			else if (event.mouseButton.button == sf::Mouse::Right)
			{
				action = 1;
			}

			// Click on Nothing
			{
				auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);

				en::F32 sqrCP = d.getSquaredLength();
				const en::Vector2f dNothing = en::toEN(nothingPositionComponent.getPosition() + sf::Vector2f(0.0f, -8.0f)) - pPos;
				en::F32 sqrNP = dNothing.getSquaredLength();
				bool validDirection = false;
				if (sqrNP > 0.1f && sqrCP > 0.1f)
				{
					validDirection = d.normalized().dotProduct(dNothing.normalized()) > 0.8f;
				}
				sf::FloatRect aabb = nothingPositionComponent.getTransform().transformRect(GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).body.getGlobalBounds());
				if (validDirection && aabb.contains(en::toSF(cPos)))
				{
					if (action == 0)
					{
						if (!GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isKO())
						{
							GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).life -= GameSingleton::hitLife;
							GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits++;
							if (GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits >= 3)
							{
								if (tutorialState == 2)
								{
									mText.handleEvent(NothingTextSystem::Event::Tutorial3);
									tutorialState++;
								}
								GameSingleton::playSound(GameSingleton::mKnockoutSound);
								GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimLoop(HumanComponent::Animation::KO);
								GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits = 0;
								GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::seconds(5.f);
								clickHandled = true;
							}
							else
							{
								GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimOnce(HumanComponent::Animation::Hitted);
							}
						}
					}
					if (action == 1 && GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isKO() && GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).life >= 0.12f)
					{
						GameSingleton::playSound(GameSingleton::mChopSound);
						GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity).playAnimOnce(HumanComponent::Animation::Chop);
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimLoop(HumanComponent::Animation::Chopped);
						GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity).chopping = true;
						GameSingleton::world.get<en::RenderableComponent>(GameSingleton::nothingEntity).z = 1;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).chopped = true;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).hits = 0;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::Time::Zero;
						clickHandled = true;
					}
				}
			}

			// Click on AI
			if (!clickHandled)
			{
				auto view = GameSingleton::world.view<HumanComponent, en::PositionComponent>();
				for (auto entity : view)
				{
					if (entity == GameSingleton::playerEntity || entity == GameSingleton::nothingEntity)
						continue;
					auto& position = view.get<en::PositionComponent>(entity);
					auto& human = view.get<HumanComponent>(entity);
					sf::FloatRect aabb = position.getTransform().transformRect(human.body.getGlobalBounds());
					if (aabb.contains(en::toSF(cPos)))
					{
						if (!clickHandled && action == 0)
						{
							human.life -= GameSingleton::hitLife;
							if (GameSingleton::world.has<AIComponent>(entity))
							{
								auto& ai = GameSingleton::world.get<AIComponent>(entity);
								ai.randomMvtTimer = ai.changeRandomMvtTime * 2.0f;
								auto& vel = GameSingleton::world.get<VelocityComponent>(entity);
								vel.velocity = en::Vector2f::zero;
							}
							human.playAnimOnce(HumanComponent::Animation::Hitted);
						}
						clickHandled = true;
						break;
					}
				}
			}
		}
	}

	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		GameSingleton::clear();
		getApplication().getAudio().stop();

		getApplication().clearStates();
		getApplication().pushState<MenuState>();
	}

	return false;
}

bool GameState::update(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

#ifdef ENLIVE_DEBUG
	devUpdate(dt);
#endif

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity);
	//auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);

	mText.update(dt);

	// AI components

	auto viewDoor = GameSingleton::world.view<DoorComponent, en::PositionComponent>();
	for (auto entity : viewDoor)
	{
		auto& doorComponent = viewDoor.get<DoorComponent>(entity);
		auto& positionComponent = viewDoor.get<en::PositionComponent>(entity);
		if (doorComponent.enabled)
		{
			doorComponent.cooldownSpawnTimer -= dt;
			if (doorComponent.cooldownSpawnTimer <= en::Time::Zero)
			{
				doorComponent.cooldownSpawnTimer = en::seconds(en::Random::get<en::F32>(15.0f, 25.0f));
				const sf::Vector2f& p = positionComponent.getPosition();
				EntityPrefab::createAI(GameSingleton::world, p.x, p.y + 16.0f);
				//GameSingleton::playSound(GameSingleton::mSelectSound);
			}
		}
	}

	velocities(dt);
	directions();

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

		en::F32 max = 0.5f;
		en::F32 t = nothingNothingComponent.throwTimer.asSeconds();
		en::F32 x = 180.0f * t / max;
		en::F32 x2 = 90.0f * t / max;
		en::F32 s = en::Math::Sin(x) * 0.5f;
		en::F32 s2 = s + en::Math::Sin(x2) * 0.5f;
		nothingHumanComponent.body.setPosition(sf::Vector2f(0.f, -s2 * 3.f));
		nothingHumanComponent.body.setScale(sf::Vector2f(1 + s, 1 + s));

		if (nothingNothingComponent.throwTimer < en::Time::Zero)
		{
			GameSingleton::playSound(GameSingleton::mKnockoutSound);
			nothingHumanComponent.playAnimOnce(HumanComponent::Animation::KO);
			nothingNothingComponent.koTimer = en::seconds(0.2f);
			nothingVelocityComponent.velocity = en::Vector2f::zero;
			nothingHumanComponent.body.setScale(sf::Vector2f(1.0f, 1.0f));
			nothingHumanComponent.body.setPosition(sf::Vector2f(0.f, 0.f));
		}
	}

	animations(dt);

	movements(dt);

	const sf::Vector2f& playerUpdatePos = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition();

	// Pieces
	auto viewPieces = GameSingleton::world.view<PieceComponent, en::PositionComponent>();
	for (auto entity : viewPieces)
	{
		auto& piece = viewPieces.get<PieceComponent>(entity);
		ENLIVE_UNUSED(piece);
		auto& position = viewPieces.get<en::PositionComponent>(entity);
		const en::Vector2f d = en::toEN(position.getPosition() - playerUpdatePos);
		if (d.getSquaredLength() < 16.0f * 16.0f)
		{
			GameSingleton::playSound(GameSingleton::mPieceGetSound);
			GameSingleton::world.destroy(entity);
		}
	}

	// Cursor/View animation
	const en::Vector2f cPos = getAdjustedCursorPos();
	const en::Vector2f pPos = en::toEN(playerUpdatePos) + en::Vector2f(0.0f, -8.0f);
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

	const en::F32 regenNothingPercentLifePerSeconds = 0.02f;
	const en::F32 regenPlayerPercentLifePerSeconds = 0.005f;

	// Remove died humans
	auto viewHuman = GameSingleton::world.view<HumanComponent>();
	for (auto entity : viewHuman)
	{
		auto& human = viewHuman.get(entity);

		if (entity == GameSingleton::nothingEntity)
		{
			human.life = en::Math::Min(human.life + regenNothingPercentLifePerSeconds * dt.asSeconds(), 1.0f);
		}
		if (entity == GameSingleton::playerEntity)
		{
			human.life = en::Math::Min(human.life + regenPlayerPercentLifePerSeconds * dt.asSeconds(), 1.0f);
		}

		if (human.life < 0.0f)
		{
			human.life = 0.0f;
		}

		if (human.life <= 0.0f)
		{
			if (entity == GameSingleton::playerEntity)
			{
				
			}
			else if (entity == GameSingleton::nothingEntity)
			{

			}
			else
			{
				GameSingleton::world.destroy(entity);
			}
		}
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
			ENLIVE_UNUSED(renderable);
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
				auto& human = GameSingleton::world.get<HumanComponent>(entity);
				en::U8 lifeP = (en::U8)(255 * human.life);
				sf::Color color;
				color.r = 255;
				color.g = lifeP;
				color.b = lifeP;
				human.body.setColor(color);
				target.draw(human.body, states);
			}
			if (GameSingleton::world.has<PropsComponent>(entity))
			{
				target.draw(GameSingleton::world.get<PropsComponent>(entity).sprite, states);
			}
			if (GameSingleton::world.has<PieceComponent>(entity))
			{
				target.draw(GameSingleton::world.get<PieceComponent>(entity).sprite, states);
			}
		}
	}

	target.draw(mCursor);

	getWindow().applyMainView();

	mText.render(target);
}

en::Window& GameState::getWindow()
{
	return getApplication().getWindow();
}

en::Vector2f GameState::getAdjustedCursorPos()
{
	const en::Vector2f mPos = getApplication().getWindow().getCursorPositionView(GameSingleton::mView);
	const en::Vector2f pPos = en::toEN(GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity).getPosition()) + en::Vector2f(0.0f, -8.0f);
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

void GameState::velocities(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& playerVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::playerEntity);
	auto& playerPlayerComponent = GameSingleton::world.get<PlayerComponent>(GameSingleton::playerEntity);
	auto& playerHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity);

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);
	//auto& nothingHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity);

	// AI
	auto view = GameSingleton::world.view<VelocityComponent, AIComponent, HumanComponent, en::PositionComponent>();
	for (auto entity : view)
	{
		auto& velocity = view.get<VelocityComponent>(entity);
		auto& ai = view.get<AIComponent>(entity);
		auto& human = view.get<HumanComponent>(entity);
		auto& position = view.get<en::PositionComponent>(entity);
		if (entity == GameSingleton::playerEntity || entity == GameSingleton::nothingEntity)
		{
			continue;
		}
		ai.randomMvtTimer -= dt;
		ai.cooldownHitTimer -= dt;
		en::Vector2f delta = en::toEN(playerPositionComponent.getPosition() - position.getPosition());
		if (delta.getSquaredLength() > 10.f * 10.f)
		{
			if (ai.randomMvtTimer <= en::Time::Zero)
			{
				en::Vector2f randomVel;
				randomVel.x = ai.randomEngine.get<en::F32>(-1.f, 1.f);
				randomVel.y = ai.randomEngine.get<en::F32>(-1.f, 1.f);
				velocity.velocity = 0.7f * (delta.normalized() * 0.7f + 0.3f * randomVel);
				ai.randomMvtTimer = ai.changeRandomMvtTime;
			}
		}
		else
		{
			velocity.velocity = en::Vector2f::zero;
			ai.randomMvtTimer = en::Time::Zero;

			if (ai.cooldownHitTimer <= en::Time::Zero)
			{
				GameSingleton::playSound(GameSingleton::mHitSound);
				ai.cooldownHitTimer = en::seconds(0.5f);
				human.playAnimOnce(HumanComponent::Animation::HitIdle);

				playerHumanComponent.life -= GameSingleton::hitLife;
				if (!playerPlayerComponent.chopping)
				{
					playerHumanComponent.playAnimOnce(HumanComponent::Animation::Hitted);
				}
			}
		}
	}

	// Player
	if (playerHumanComponent.currentAnimation != HumanComponent::Animation::Chop)
	{
		playerVelocityComponent.velocity = playerPlayerComponent.getVelocity();
		if (playerVelocityComponent.velocity.getSquaredLength() > 0.1f && tutorialState == 0)
		{
			mText.handleEvent(NothingTextSystem::Event::Tutorial2);
			tutorialState++;
		}
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
		static const en::Time changeRandomMvtTime = en::seconds(1.0f);
		static en::Time randomMvtTimer = en::Time::Zero;
		static en::RandomEngine randomEngine(123456);
		randomMvtTimer -= dt;
		en::Vector2f delta = en::toEN(playerPositionComponent.getPosition() - nothingPositionComponent.getPosition());
		if (delta.getSquaredLength() > 32.f * 32.f)
		{
			nothingVelocityComponent.velocity = delta.normalized();
			randomMvtTimer = en::Time::Zero;
		}
		else if (randomMvtTimer <= en::Time::Zero)
		{
			randomMvtTimer = changeRandomMvtTime;
			nothingVelocityComponent.velocity = en::Vector2f::zero;
			en::Vector2f randomVel;
			randomVel.x = randomEngine.get<en::F32>(-1.f, 1.f);
			randomVel.y = randomEngine.get<en::F32>(-1.f, 1.f);
			if (delta.getSquaredLength() > 0.1f)
			{
				en::U32 r = randomEngine.get<en::U32>(0, 4);
				if (r > 1)
				{
					nothingVelocityComponent.velocity = 0.9f * (delta.normalized() * 0.6f + 0.4f * randomVel.normalized());
				}
				else
				{
					nothingVelocityComponent.velocity = en::Vector2f::zero;
					randomMvtTimer = changeRandomMvtTime * en::Random::get<en::F32>(1.f, 3.f);;
				}
			}
			else
			{
				nothingVelocityComponent.velocity = 0.9f * randomVel;
			}
		}
		else
		{
			// Stay as it is
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
	ENLIVE_PROFILE_FUNCTION();

	auto& playerPositionComponent = GameSingleton::world.get<en::PositionComponent>(GameSingleton::playerEntity);
	auto& playerHumanComponent = GameSingleton::world.get<HumanComponent>(GameSingleton::playerEntity);

	auto& nothingNothingComponent = GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity);
	auto& nothingVelocityComponent = GameSingleton::world.get<VelocityComponent>(GameSingleton::nothingEntity);

	auto view = GameSingleton::world.view<HumanComponent, VelocityComponent, en::PositionComponent>();
	for (auto entity : view)
	{
		auto& human = view.get<HumanComponent>(entity);
		auto& velocity = view.get<VelocityComponent>(entity);
		auto& position = view.get<en::PositionComponent>(entity);
		human.walking = velocity.velocity.getSquaredLength() > 0.1f;

		if (entity == GameSingleton::playerEntity)
		{
			en::F32 angle = 0.0f;
			en::Vector2f deltaView = getAdjustedCursorPos() - (en::toEN(playerPositionComponent.getPosition()) + en::Vector2f(0.0f, -8.0f));
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
			en::F32 angle = 0.0f;
			en::Vector2f deltaView = en::toEN(playerPositionComponent.getPosition() - position.getPosition());
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
	}
}

void GameState::animations(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

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
		else if (human.currentAnimation == HumanComponent::Animation::HitIdle || human.currentAnimation == HumanComponent::Animation::HitWalking)
		{
			if (human.walking && human.currentAnimation == HumanComponent::Animation::HitIdle)
			{
				human.currentAnimation = HumanComponent::Animation::HitWalking;
			}
			else if (!human.walking && human.currentAnimation == HumanComponent::Animation::HitWalking)
			{
				human.currentAnimation = HumanComponent::Animation::HitIdle;
			}
		}
		else if (human.currentAnimation == HumanComponent::Animation::ChoppingIdle || human.currentAnimation == HumanComponent::Animation::ChoppingWalking)
		{
			if (human.walking && human.currentAnimation == HumanComponent::Animation::ChoppingIdle)
			{
				human.playAnimLoop(HumanComponent::Animation::ChoppingWalking);
			}
			else if (!human.walking && human.currentAnimation == HumanComponent::Animation::ChoppingWalking)
			{
				human.playAnimLoop(HumanComponent::Animation::ChoppingIdle);
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
}

void GameState::movements(en::Time dt)
{
	ENLIVE_PROFILE_FUNCTION();

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
				const sf::Vector2f sfPos = en::toSF(newPos);
				position.setPosition(sfPos);

				if (entity == GameSingleton::nothingEntity && GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isThrowing())
				{
					bool hitAI = false;
					auto viewAI = GameSingleton::world.view<en::PositionComponent, AIComponent, HumanComponent>();
					for (auto aiEntity : viewAI)
					{
						auto& aiPosition = viewAI.get<en::PositionComponent>(aiEntity);
						auto& ai = viewAI.get<AIComponent>(aiEntity);
						ENLIVE_UNUSED(ai);
						auto& human = viewAI.get<HumanComponent>(aiEntity);

						const sf::FloatRect aabb = aiPosition.getTransform().transformRect(human.body.getGlobalBounds());
						if (aabb.contains(sfPos))
						{
							human.life -= GameSingleton::aiNothingLife;
							hitAI = true;
						}
					}
					if (hitAI)
					{
						GameSingleton::playSound(GameSingleton::mNothingAISound);
						GameSingleton::playSound(GameSingleton::mKnockoutSound);
						velocity.velocity = en::Vector2f::zero;
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimLoop(HumanComponent::Animation::KO);
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).body.setScale(sf::Vector2f(1.0f, 1.0f));
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).body.setPosition(sf::Vector2f(0.f, 0.f));
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).life -= GameSingleton::nothingAILife;
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::seconds(1.5f);
						GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::Time::Zero;
					}
				}
			}
			else if (velocity.velocity.x != 0.0f && velocity.velocity.y != 0.0f)
			{
				if (entity == GameSingleton::nothingEntity && GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).isThrowing())
				{
					GameSingleton::playSound(GameSingleton::mKnockoutSound);
					velocity.velocity = en::Vector2f::zero;
					GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).playAnimLoop(HumanComponent::Animation::KO);
					GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).body.setScale(sf::Vector2f(1.0f, 1.0f));
					GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).body.setPosition(sf::Vector2f(0.f, 0.f));
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).koTimer = en::seconds(1.5f);
					GameSingleton::world.get<NothingComponent>(GameSingleton::nothingEntity).throwTimer = en::Time::Zero;

					bool wasAProps = false;
					auto viewDestr = GameSingleton::world.view<en::PositionComponent, PropsComponent>();
					for (auto propsEntity : viewDestr)
					{
						auto& propsPosition = viewDestr.get<en::PositionComponent>(propsEntity);
						auto& props = viewDestr.get<PropsComponent>(propsEntity);

						if (!props.destructed)
						{
							const sf::Vector2f& propsPos = propsPosition.getPosition();
							en::Vector2f d = newPos - en::toEN(propsPos);
							if (d.getSquaredLength() < 11.5f * 11.5f)
							{
								wasAProps = true;
								props.destruct();

								if (mPieceGenerator.get<en::U32>(0, 4) == 1)
								{
									GameSingleton::playSound(GameSingleton::mPieceSound);
									EntityPrefab::createPiece(GameSingleton::world, propsPos.x, propsPos.y);
								}
							}
						}
					}
					if (wasAProps)
					{
						GameSingleton::playSound(GameSingleton::mDestrPropsSound);
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).life -= GameSingleton::nothingPropsLife;
					}
					else
					{
						GameSingleton::playSound(GameSingleton::mNothingWallSound);
						GameSingleton::world.get<HumanComponent>(GameSingleton::nothingEntity).life -= GameSingleton::nothingMurLife;
					}

					const en::Vector2f newPosAfterDestr = pos + en::Vector2f(velX, velY) * 3.0f;
					if (!isCollision(newPosAfterDestr))
					{
						position.setPosition(en::toSF(newPosAfterDestr));
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
