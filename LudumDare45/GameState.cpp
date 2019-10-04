#include "GameState.hpp"

#include <Enlivengine/Core/Components.hpp>
#include <Enlivengine/Graphics/DebugDraw.hpp>

#include "EntityPrefab.hpp"
#include "EditorComponents.hpp"
#include "GameSingleton.hpp"

GameState::GameState(en::StateManager& manager)
	: en::State(manager)
{
	GameSingleton::clear();

	// View settings
	getView().setCenter(0.0f, 0.0f);
	getView().setSize(getApplication().getWindow().getSize());
	//getView().setZoom(2.0f);
	//getApplication().getWindow().setMainView(en::View(0.f, 0.f, 1024.f, 768.f));

	//getApplication().getTextures().create("atmog", en::TextureLoader::loadFromFile("Assets/Textures/atmog.png"));
	//asteroidTextureId = getApplication().getTextures().create("asteroids", en::TextureLoader::loadFromFile("Assets/Textures/asteroids.png"));
	//fontId = getApplication().getFonts().create("font", en::FontLoader::loadFromFile("Assets/Fonts/ErasBoldITC.ttf"));

#ifdef ENLIVE_ENABLE_IMGUI
	GameSingleton::currentEntity = entt::null;
	EditorComponents::registerToEditor(GameSingleton::worldEditor);
#endif

	// Audio
	getApplication().getAudio().setGlobalVolume(3.0f);

	// Music
	getApplication().getAudio().createMusic("mainTheme", "Assets/Musics/MainTheme.ogg");
	getApplication().getAudio().playMusic("mainTheme");

	//getApplication().getAudio().createSound("click", "Assets/Sounds/click.wav");

	//background.setTexture(getApplication().getTextures().get("atmog"));

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
	const en::Vector2f mPos = getApplication().getWindow().getCursorPosition();

	if (!clickHandled && event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right)
	{
		GameSingleton::playClick();
		EntityPrefab::createEntity(GameSingleton::world, mPos.x, mPos.y);
		
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

	return false;
}

void GameState::render(sf::RenderTarget& target)
{
	ENLIVE_PROFILE_FUNCTION();

	{
		ENLIVE_PROFILE_SCOPE(RenderSort);
		GameSingleton::world.sort<en::RenderableComponent>([](const auto& lhs, const auto& rhs)
		{
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

			if (GameSingleton::world.has<en::SpriteComponent>(entity))
			{
				target.draw(GameSingleton::world.get<en::SpriteComponent>(entity), states);
			}
			// TODO : AnimationComponent rendering (or remove)
			if (GameSingleton::world.has<en::AnimationComponent>(entity))
			{
			}
			if (GameSingleton::world.has<en::TextComponent>(entity))
			{
				target.draw(GameSingleton::world.get<en::TextComponent>(entity), states);
			}
		}
	}

	{
#ifdef ENLIVE_DEBUG
		en::DebugDraw::render(target);
#endif // ENLIVE_DEBUG
	}
}

en::Window& GameState::getWindow()
{
	return getApplication().getWindow();
}

en::View& GameState::getView()
{
	return getApplication().getWindow().getMainView();
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
