
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

int main()
{
	en::LogManager::initialize();

	en::Application app;
	app.getWindow().create(sf::VideoMode(1024, 768), "StoryOfNothing", sf::Style::Titlebar | sf::Style::Close);
	app.getWindow().getMainView().setCenter({ 512.0f, 384.0f });
	app.getWindow().getMainView().setSize({ 1024.0f, 768.0f });
	app.getScreenshotSystem().setScreenshotPath("");

	GameSingleton::loadResourcesMain(app);
	GameSingleton::loadResourcesGame();

	GameSingleton::setCursor(true);

	app.start<IntroState>();

	return 0;
}
