
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

void Test(const en::MetaDataType& typeInfo)
{
	/*
	std::cout << typeInfo.GetName() << " (" << typeInfo.GetID() << ") : " << std::endl;
	std::cout << "   - size of " << typeInfo.GetSize() << ", with align(" << typeInfo.GetAlignement() << ") and padding(" << typeInfo.GetPadding() << ")" << std::endl;
	std::cout << "   - signature : " << typeInfo.GetSignature() << std::endl;
	if (typeInfo.HasProperties())
	{
		const en::U32 propertyCount = typeInfo.GetPropertyCount();
		std::cout << "   - " << propertyCount << " properties :" << std::endl;
		for (en::U32 i = 0; i < propertyCount; ++i)
		{
			const en::MetaDataProperty& propertyInfo = typeInfo.GetPropertyByIndex(i);
			std::cout << "       - " << propertyInfo.GetName() << " (" << propertyInfo.GetID() << ") of type " << en::MetaDataTypes::GetTypeByIndex(propertyInfo.GetTypeIndex()).GetName() << " at offset " << propertyInfo.GetOffset() << std::endl;
		}
	}
	else
	{
		std::cout << "   - no properties" << std::endl;
	}
	std::cout << std::endl;
	*/
}

int main()
{
	/*
	static_assert(en::MetaDataTypes::GetType("int").GetName() == "int");
	static_assert(en::MetaDataTypes::GetType("int").GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<int>().GetName() == "int");
	static_assert(en::MetaDataTypes::GetType<int>().GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<float>().GetName() == "float");
	static_assert(en::MetaDataTypes::GetType<float>().GetSize() == 4);
	static_assert(en::MetaDataTypes::GetType<int>() != en::MetaDataTypes::GetType<float>());
	for (en::U32 i = 0; i < en::MetaDataTypes::GetTypeCount(); ++i)
	{
		Test(en::MetaDataTypes::GetTypeByIndex(i));
	}
	*/


	static_assert(en::TestClassA::GetStaticMetaData().GetSize() == 8);
	std::cout << en::TestClassA::GetStaticMetaData().GetSize() << std::endl;

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
