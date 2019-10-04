
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"

int main(int argc, char** argv)
{
	en::LogManager::initialize();

	en::Application app;

	// GameConfig::loadFromFile();
	
	app.getWindow().create(sf::VideoMode(1024, 768), "LudumDare45");
	app.getWindow().setScreenshotPath("Assets/Screenshots/");

	GameSingleton::loadResourcesMain(app);

	app.start<GameState>();

	return 0;
}
