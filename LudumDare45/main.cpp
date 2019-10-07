
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

int main(int argc, char** argv)
{
	en::LogManager::initialize();

	en::Application app;
	app.getWindow().create(sf::VideoMode(1024, 768), "StoryOfNothing", sf::Style::Titlebar | sf::Style::Close);
	app.getWindow().setScreenshotPath("Assets/Screenshots/");
	app.getWindow().getMainView().setCenter(512.0f, 384.0f);
	app.getWindow().getMainView().setSize(en::Vector2f(1024, 768));

	GameSingleton::loadResourcesMain(app);
	GameSingleton::loadResourcesGame();

	GameSingleton::setCursor(true);

	app.start<IntroState>();

	return 0;
}
