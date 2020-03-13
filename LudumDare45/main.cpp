
#include <Enlivengine/System/Log.hpp>
#include <Enlivengine/Application/StateManager.hpp>
#include <Enlivengine/Application/Application.hpp>

#include "GameSingleton.hpp"
#include "GameConfig.hpp"

#include "GameState.hpp"
#include "IntroState.hpp"

// Test MetaData
#include <iostream>
#include <string>
#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/MetaData/TestClassB.hpp>
#include <Enlivengine/MetaData/TestClassC.hpp>
#include <Enlivengine/MetaData/MetaData.hpp>

void TestMetaData(const en::MetaDataType& typeInfo)
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
			std::cout << "       - " << propertyInfo.GetName() << " (" << propertyInfo.GetID() << ") of type " << propertyInfo.GetType().GetName() << " at offset " << propertyInfo.GetOffset() << " of size " << propertyInfo.GetSize() << std::endl;
		}
	}
	else
	{
		std::cout << "   - no properties" << std::endl;
	}
	std::cout << std::endl;
}

/* Helper */
std::string indentString = "";
en::U32 indent = 0;
void Indent()
{
    indent++;
    indentString += "  ";
}
void Unindent()
{
    indent--;
    indentString.pop_back();
    indentString.pop_back();
}
const void* GetSubObjectAtOffset(const void* object, en::U32 offset)
{
    return (const void*)((const en::U8*)(object) + static_cast<std::size_t>(offset));
}
/* Serialization */
void TestSerialization(const void* object, const en::MetaDataType& metaDataType)
{
    if ((metaDataType.GetTraits() & en::TypeTraits_Enum) > 0)
    {
        assert(metaDataType.GetEnumType() != nullptr);
        assert(metaDataType.GetSize() == ENLIVE_SIZE_OF(en::U32));
        const en::U32 value = *(const en::U32*)object;
        const en::MetaDataEnumValue& enumValue = metaDataType.GetEnumType()->GetValueByValue(value);
        std::cout << metaDataType.GetEnumType()->GetName() << "::" << enumValue.GetName();
    }
    else if ((metaDataType.GetTraits() & en::TypeTraits_Primitive) > 0)
    {
        #define PrimType(Type) case en::MetaData::GetType<Type>().GetID(): \
                                    assert(metaDataType.GetSize() == ENLIVE_SIZE_OF(Type)); \
                                    std::cout << *(const Type*)object; \
                                break;
        switch (metaDataType.GetID())
        {
            PrimType(en::U32);
            PrimType(en::I32);
            PrimType(en::F32);
            case en::MetaData::GetType<bool>().GetID():
            {
                assert(metaDataType.GetSize() == ENLIVE_SIZE_OF(bool));
                std::cout << ((*(const bool*)object) ? "true" : "false");
            } break;
            default: assert(false); break;
        }
        #undef PrimType
    }
    else if ((metaDataType.GetTraits() & en::TypeTraits_Class) > 0)
    {
        std::cout << "{" << std::endl;
        Indent();

        /*
        if (metaDataType.GetParent() != nullptr)
        {
            TestSerialization(object, *metaDataType.GetParent());
        }
        */

        const en::U32 propertyCount = metaDataType.GetPropertyCount();
        for (en::U32 i = 0; i < propertyCount; ++i)
        {
            const en::MetaDataProperty& metaDataProperty = metaDataType.GetPropertyByIndex(i);
            const en::U32 offset = metaDataProperty.GetOffset();
            std::cout << indentString << "\"" << metaDataProperty.GetName() << "\": ";

            if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Array) == en::TypeTraits_Array)
            {
                /*
                assert(false);
                std::cout << std::endl;
                const en::U32 elementCount = metaDataProperty.GetElementCount();
                const en::U32 elementSize = metaDataProperty.GetType().GetSize();
                for (en::U32 j = 0; j < elementCount; ++j)
                {
                    std::cout << j << " ";
                    TestSerialization(GetSubObjectAtOffset(object, offset + elementSize * j), metaDataProperty.GetType());
                }
                */
            }
            else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Pointer) == en::TypeTraits_Pointer)
            {
                assert(metaDataProperty.GetSize() == ENLIVE_SIZE_OF(void*));
                if ((metaDataProperty.GetTypeTraits() & (en::TypeTraits_Primitive | en::TypeTraits_Enum)) > 0)
                {
                    if (const void* ptr = *(const void**)GetSubObjectAtOffset(object, offset))
                    {
                        TestSerialization(ptr, metaDataProperty.GetType());
                    }
                    else
                    {
                        std::cout << "null" << std::endl;
                    }
                }
                else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Class) == en::TypeTraits_Class)
                {
                    if (const void* subObject = *(const void**)GetSubObjectAtOffset(object, offset))
                    {
                        TestSerialization(subObject, metaDataProperty.GetType());
                    }
                    else
                    {
                        std::cout << "null" << std::endl;
                    }
                }
                else
                {
                    assert(false);
                }
            }
            else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Reference) == en::TypeTraits_Reference)
            {
                assert(false);
            }
            else if ((metaDataProperty.GetTypeTraits() & (en::TypeTraits_Primitive | en::TypeTraits_Enum)) > 0)
            {
                TestSerialization(GetSubObjectAtOffset(object, offset), metaDataProperty.GetType());
                if (i < propertyCount - 1)
                {
                    std::cout << ",";
                }
                std::cout << std::endl;
            }
            else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Class) == en::TypeTraits_Class)
            {
                std::cout << std::endl;
                TestSerialization((const void*)GetSubObjectAtOffset(object, offset), metaDataProperty.GetType());
            }
            else
            {
                assert(false);
            }
        }

        Unindent();
        std::cout << indentString << "}" << std::endl;
    }
    else
    {
        assert(false);
    }
}

int main()
{
	std::cout << "----------------------------" << std::endl;
	constexpr en::U32 typeCount = en::MetaData::GetTypeCount();
	for (en::U32 i = 0; i < typeCount; ++i)
	{
		TestMetaData(en::MetaData::GetTypeFromIndex(i));
	}

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

	en::TestClassA testA;
	testA.SetA(10);
	testA.SetB(-123);
	testA.SetC(3.14152f);
    testA.SetD(en::MyEnum::A);
    testA.SetE(true);
    TestSerialization(&testA, en::TestClassA::GetStaticMetaData());

    std::cout << "----------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << "----------------------------" << std::endl;
    
    en::TestClassA testA2;
    testA2.SetA(1);
    testA2.SetB(-321);
    testA2.SetC(2.12102101f);
    testA2.SetD(en::MyEnum::C);
    testA2.SetE(false);
    TestSerialization(&testA2, en::TestClassA::GetStaticMetaData());

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

    static const bool testNullptr = true; // Change this to ensure nullptr are also tested
    en::F32 bD = 0.1234f;
    en::MyEnum bE = en::MyEnum::B;
    en::F32 x = 10.0f;
    en::F32 y = 10.0f;
	en::TestClassB testB;
	testB.SetA(testNullptr ? nullptr : &testA);
	testB.SetB(123456789.0f);
    testB.SetC(42);
    testB.SetD(!testNullptr ? nullptr : &bD);
    testB.SetE(testNullptr ? nullptr : &bE);
    testB.SetF(0, testA2);
    testB.SetF(1, testA);
    testB.SetF(2, testA2);
    testB.SetG(0, &x);
    testB.SetG(1, &y);
	TestSerialization(&testB, en::TestClassB::GetStaticMetaData());
    
	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
    std::cout << "----------------------------" << std::endl;

    /*

	en::TestClassC testC;
	testC.en::TestClassA::SetA(13);
	testC.en::TestClassA::SetB(-13);
    testC.en::TestClassA::SetC(13.31f);
    testC.en::TestClassA::SetD(en::MyEnum::C);
	testC.SetB(987654321.0f);
	testC.SetC(24);
	TestSerialization(&testC, en::TestClassC::GetStaticMetaData());

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

    */

    /*
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
    */

	return 0;
}
