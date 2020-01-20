
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

int main()
{
	en::Application::GetInstance().Initialize();

	en::Application& app = en::Application::GetInstance();
	app.GetWindow().create(sf::VideoMode(1024, 768), "StoryOfNothing", sf::Style::Titlebar | sf::Style::Close);
	app.GetWindow().getMainView().setCenter({ 512.0f, 384.0f });
	app.GetWindow().getMainView().setSize({ 1024.0f, 768.0f });
	en::PathManager::GetInstance().SetScreenshotPath("Screenshots/");

	GameSingleton::loadResourcesMain(app);
	GameSingleton::loadResourcesGame();

	GameSingleton::setCursor(true);

	app.Start<IntroState>();

	return 0;
}
