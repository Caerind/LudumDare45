
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

// Test MetaData
#include <iostream>
#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/MetaData/TestClassB.hpp>
#include <Enlivengine/MetaData/TestClassC.hpp>
#include <Enlivengine/MetaData/MetaData.hpp>

void Test(const en::MetaDataType& typeInfo)
{
	std::cout << typeInfo.GetName() << " (" << typeInfo.GetID() << ") : " << std::endl;
    std::cout << "   - size of " << typeInfo.GetSize() << ", with align(" << typeInfo.GetAlignment() << ")" << std::endl;
    if (typeInfo.GetParent() != nullptr)
    {
        std::cout << "   - parent class : " << typeInfo.GetParent()->GetName() << std::endl;
    }
    std::cout << "   - signature : " << typeInfo.GetSignature() << std::endl;
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

const void* GetSubObjectAtOffset(const void* object, en::U32 offset)
{
	return (const void*)((const en::U8*)(object) + static_cast<std::size_t>(offset));
}

void TestSerialization(const void* object, const en::MetaDataType& metaDataType)
{
	std::cout << "-(" << metaDataType.GetName() << ")-" << std::endl;

	if (metaDataType.GetParent() != nullptr)
	{
		TestSerialization(object, *metaDataType.GetParent());
	}

	const en::U32 propertyCount = metaDataType.GetPropertyCount();
	for (en::U32 i = 0; i < propertyCount; ++i)
	{
		const en::MetaDataProperty& metaDataProperty = metaDataType.GetPropertyByIndex(i);
		const en::U32 offset = metaDataProperty.GetOffset();
		std::cout << metaDataProperty.GetName() << " : ";
		if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Primitive) == en::TypeTraits_Primitive)
		{
			switch (metaDataProperty.GetType().GetID())
			{
			case en::MetaData::GetType<en::U32>().GetID(): std::cout << *(const en::U32*)GetSubObjectAtOffset(object, offset) << " (U32)" << std::endl; break;
			case en::MetaData::GetType<en::I32>().GetID(): std::cout << *(const en::I32*)GetSubObjectAtOffset(object, offset) << " (I32)" << std::endl; break;
			case en::MetaData::GetType<en::F32>().GetID(): std::cout << *(const en::F32*)GetSubObjectAtOffset(object, offset) << " (F32)" << std::endl; break;
			default: assert(false); break;
			}
		}
		else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Class) == en::TypeTraits_Class)
		{
			std::cout << std::endl;
			TestSerialization(GetSubObjectAtOffset(object, offset), metaDataProperty.GetType());
		}
		else
		{
			assert(false);
		}
	}

	std::cout << "- - - -" << std::endl;
}

int main()
{
	std::cout << "----------------------------" << std::endl;
	constexpr en::U32 typeCount = en::MetaData::GetTypeCount();
	for (en::U32 i = 0; i < typeCount; ++i)
	{
		Test(en::MetaData::GetTypeFromIndex(i));
	}

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

	en::TestClassA testA;
	testA.SetA(10);
	testA.SetB(-123);
	testA.SetC(3.14152f);
	TestSerialization(&testA, en::TestClassA::GetStaticMetaData());

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

	en::TestClassB testB;
	testB.SetA(testA);
	testB.SetB(123456789.0f);
	testB.SetC(42);
	TestSerialization(&testB, en::TestClassB::GetStaticMetaData());

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

	en::TestClassC testC;
	testC.en::TestClassA::SetA(13);
	testC.en::TestClassA::SetB(-13);
	testC.en::TestClassA::SetC(13.31f);
	testC.SetB(987654321.0f);
	testC.SetC(24);
	TestSerialization(&testC, en::TestClassC::GetStaticMetaData());

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

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
