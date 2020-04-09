
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

#ifdef ENLIVE_ENABLE_METADATA

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
	if (typeInfo.HasAttributes())
	{
		std::cout << "   - attributes : ";
		if (typeInfo.IsTransient())
		{
			std::cout << "Transient ";
		}
		if (typeInfo.HasCustomSerialization())
		{
			std::cout << "CustomSerialization ";
		}
		std::cout << std::endl;
	}
	if (typeInfo.IsTemplatedClass())
	{
		std::cout << "   - template ID : " << typeInfo.GetTemplateID() << std::endl;
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
	if (metaDataType.HasCustomSerialization())
	{
		using AnyTemplateType = en::U32; // Used for readability below

		if (metaDataType.GetID() == en::MetaData<en::Time>::Get().GetID())
		{
			std::cout << ((const en::Time*)object)->asMilliseconds() << "ms";
		}
		else if (metaDataType.GetTemplateID() == en::MetaData<en::Array<AnyTemplateType>>::Get().GetTemplateID())
		{
			const en::U32 size = *(const en::U32*)GetSubObjectAtOffset(object, metaDataType.GetPropertyByIndex(1).GetOffset());
			if (size > 0)
			{
				std::cout << "[" << std::endl;
				Indent();
				const en::U32 arrayOffset = metaDataType.GetPropertyByIndex(0).GetOffset();
				if (const void* arrayPtr = *(const void**)GetSubObjectAtOffset(object, arrayOffset))
				{
					const en::U32 elementSize = metaDataType.GetPropertyByIndex(0).GetType().GetSize();
					for (en::U32 i = 0; i < size; ++i)
					{
						std::cout << indentString;
						TestSerialization(GetSubObjectAtOffset(arrayPtr, elementSize * i), metaDataType.GetPropertyByIndex(0).GetType());
						if (i < size - 1)
						{
							std::cout << ",";
						}
						std::cout << std::endl;
					}
				}
				else
				{
					std::cout << "null";
				}
				Unindent();
				std::cout << indentString << "]";
			}
			else
			{
				std::cout << "[]";
			}
		}
		else
		{
			assert(false);
		}
	}
	else
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
#define PrimType(Type) \
			case en::MetaData<Type>::Get().GetID(): \
			{ \
				assert(metaDataType.GetSize() == ENLIVE_SIZE_OF(Type)); \
                std::cout << *(const Type*)object; \
            } break;
			switch (metaDataType.GetID())
			{
				PrimType(en::I8);
				PrimType(en::U8);
				PrimType(en::I16);
				PrimType(en::U16);
				PrimType(en::I32);
				PrimType(en::U32);
				PrimType(en::I64);
				PrimType(en::U64);
				PrimType(en::F32);
				PrimType(en::F64);
			case en::MetaData<bool>::Get().GetID():
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

			const en::U32 propertyCount = metaDataType.GetPropertyCount();
			for (en::U32 i = 0; i < propertyCount; ++i)
			{
				const en::MetaDataProperty& metaDataProperty = metaDataType.GetPropertyByIndex(i);
				const en::U32 offset = metaDataProperty.GetOffset();
				std::cout << indentString << "\"" << metaDataProperty.GetName() << "\": ";

				if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Array) == en::TypeTraits_Array)
				{
					std::cout << "[" << std::endl;
					Indent();

					const en::U32 elementCount = metaDataProperty.GetElementCount();
					const en::U32 elementSize = metaDataProperty.GetElementSize();
					for (en::U32 j = 0; j < elementCount; ++j)
					{
						std::cout << indentString;

						if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Pointer) == en::TypeTraits_Pointer)
						{
							if (const void* ptr = *(const void**)GetSubObjectAtOffset(object, offset + elementSize * j))
							{
								TestSerialization(ptr, metaDataProperty.GetType());
							}
							else
							{
								std::cout << "null";
							}
						}
						else
						{
							TestSerialization(GetSubObjectAtOffset(object, offset + elementSize * j), metaDataProperty.GetType());
						}

						if (j < elementCount - 1)
						{
							std::cout << ",";
						}
						std::cout << std::endl;
					}

					Unindent();
					std::cout << indentString << "]";
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
							std::cout << "null";
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
							std::cout << "null";
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
				}
				else if ((metaDataProperty.GetTypeTraits() & en::TypeTraits_Class) == en::TypeTraits_Class)
				{
					TestSerialization((const void*)GetSubObjectAtOffset(object, offset), metaDataProperty.GetType());
				}
				else
				{
					assert(false);
				}

				if (i < propertyCount - 1)
				{
					std::cout << ",";
				}
				std::cout << std::endl;
			}

			Unindent();
			std::cout << indentString << "}";
			if (indentString.length() == 0)
			{
				std::cout << std::endl;
			}
		}
		else
		{
			assert(false);
		}
	}
}

int main()
{
	const en::Time timeStart = en::Time::now();

	std::cout << "----------------------------" << std::endl;
	{
		TestMetaData(en::MetaData<bool>::Get());
		TestMetaData(en::MetaData<en::I8>::Get());
		TestMetaData(en::MetaData<en::I16>::Get());
		TestMetaData(en::MetaData<en::U32>::Get());
		TestMetaData(en::MetaData<en::U64>::Get());

		TestMetaData(en::MetaData<en::TestClassA>::Get());
		TestMetaData(en::MetaData<en::TestClassB>::Get());
		TestMetaData(en::MetaData<TestClassC>::Get());
		TestMetaData(en::MetaData<en::Time>::Get());

		TestMetaData(en::MetaData<en::Array<en::I8>>::Get());
		TestMetaData(en::MetaData<en::Array<en::U32>>::Get());
		TestMetaData(en::MetaData<en::Array<en::TestClassA>>::Get());
	}
	/*
	constexpr en::U32 typeCount = en::MetaData::GetTypeCount();
	for (en::U32 i = 0; i < typeCount; ++i)
	{
		TestMetaData(en::MetaData::GetTypeFromIndex(i));
	}
	*/

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

    static const bool testNullptr = false; // Change this to ensure nullptr are also tested
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

	TestClassC testC;
	testC.en::TestClassA::SetA(13);
	testC.en::TestClassA::SetB(-13);
    testC.en::TestClassA::SetC(13.31f);
    testC.en::TestClassA::SetD(en::MyEnum::C);
	testC.GetB().Resize(3);
	testC.GetB()[0] = 123;
	testC.GetB()[1] = 456;
	testC.GetB()[2] = 789;
	testC.GetC().Resize(2);
	testC.GetC()[0] = testA;
	testC.GetC()[1] = testA2;
	testC.SetDuration(en::Time::now() - timeStart);
	TestSerialization(&testC, TestClassC::GetStaticMetaData());

#else // ENLIVE_ENABLE_METADATA

int main()
{

#endif // ENLIVE_ENABLE_METADATA

	std::cout << "----------------------------" << std::endl;
	std::cout << std::endl;
	std::cout << "----------------------------" << std::endl;

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
