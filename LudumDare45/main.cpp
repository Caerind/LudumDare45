
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

// Test MetaData
#include <iostream>
#include <Enlivengine/MetaData/MetaData.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/MetaData/TestClassB.hpp>
#include <Enlivengine/MetaData/TestClassC.hpp>

void Test(const en::MetaDataType& typeInfo)
{
	std::cout << typeInfo.GetName() << " (" << typeInfo.GetID() << ") : " << std::endl;
    std::cout << "   - size of " << typeInfo.GetSize() << ", with align(" << typeInfo.GetAlignment() << ")" << std::endl;
    if (typeInfo.GetParent() != nullptr)
    {
        std::cout << "   - parent class : " << typeInfo.GetParent()->GetName() << std::endl;
    }
    //std::cout << "   - signature : " << typeInfo.GetSignature() << std::endl;
	if (typeInfo.GetPropertyCount() > 0)
	{
		const en::U32 propertyCount = typeInfo.GetPropertyCount();
		std::cout << "   - " << propertyCount << " properties :" << std::endl;
		for (en::U32 i = 0; i < propertyCount; ++i)
		{
			const en::MetaDataProperty& propertyInfo = typeInfo.GetPropertyByIndex(i);
			std::cout << "       - " << propertyInfo.GetName() << " (" << propertyInfo.GetID() << ") of type " << propertyInfo.GetType().GetName() << " at offset " << propertyInfo.GetOffset() << std::endl;
		}
	}
	else
	{
		std::cout << "   - no properties" << std::endl;
	}
	std::cout << std::endl;
}

int main()
{
    Test(en::TestClassA::GetStaticMetaData());
    //Test(en::TestClassA_MetaData);
    Test(en::TestClassB::GetStaticMetaData());
    //Test(en::TestClassB_MetaData);
    Test(en::TestClassC::GetStaticMetaData());
    //Test(en::TestClassC_MetaData);


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
