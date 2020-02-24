
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

// Test MetaData
#include <Enlivengine/MetaData/MetaData.hpp>
#include <iostream>
#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/MetaData/TestClassB.hpp>

int main()
{
	static_assert(en::MetaDataTypes::GetType("int").GetName() == "int");
	static_assert(en::MetaDataTypes::GetType("int").GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<int>().GetName() == "int");
	static_assert(en::MetaDataTypes::GetType<int>().GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<float>().GetName() == "float");
	static_assert(en::MetaDataTypes::GetType<float>().GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<int>() != en::MetaDataTypes::GetType<float>());

	for (en::U32 i = 0; i < en::MetaDataTypes::GetTypeCount(); ++i)
	{
		const en::MetaDataType& typeInfo = en::MetaDataTypes::GetTypeByIndex(i);
		std::cout << typeInfo.GetName() << " (" << typeInfo.GetID() << ") : " << typeInfo.GetSize() << std::endl;
	}

	static_assert(en::TestClassA::ClassName == "en::TestClassA");
	std::cout << en::TestClassA::ClassName << " : " << en::TestClassA::ClassID << std::endl;
	for (en::U32 i = 0; i < en::TestClassA::GetPropertieCount(); ++i)
	{
		const en::MetaDataProperty& propertyInfo = en::TestClassA::GetPropertyByIndex(i);
		std::cout << propertyInfo.GetName() << " : " << propertyInfo.GetType().GetName() << std::endl;
	}

	static_assert(en::TestClassB::ClassName == "en::TestClassB");
	std::cout << en::TestClassB::ClassName << " : " << en::TestClassB::ClassID << std::endl;
	for (en::U32 i = 0; i < en::TestClassB::GetPropertieCount(); ++i)
	{
		const en::MetaDataProperty& propertyInfo = en::TestClassB::GetPropertyByIndex(i);
		std::cout << propertyInfo.GetName() << " : " << propertyInfo.GetType().GetName() << std::endl;
	}


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
