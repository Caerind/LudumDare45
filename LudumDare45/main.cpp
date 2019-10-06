
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

	LogInfo(en::LogChannel::Global, 3, "Starting...");

	en::Application app;

	// GameConfig::loadFromFile();
	
	app.getWindow().create(sf::VideoMode(1024, 768), "LudumDare45");
	app.getWindow().setScreenshotPath("Assets/Screenshots/");
	//app.getWindow().setCursorTexture("Assets/Textures/cursor.png");
	//app.getWindow().setCursorTextureRect(sf::IntRect(0, 0, 32, 32));
	//app.getWindow().setCursorOrigin({ 16,16 });
	app.getWindow().setCursor(en::Window::Cursor::None);
	app.getWindow().getMainView().setCenter(512.0f, 384.0f);
	app.getWindow().getMainView().setSize(en::Vector2f(1024, 768));

	GameSingleton::loadResourcesMain(app);

	LogInfo(en::LogChannel::Global, 3, "Main resources loaded");

	app.start<IntroState>();

	return 0;
}
