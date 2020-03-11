#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>

namespace en
{

enum class MyEnum : en::U32
{
    A,
    B,
    C
};
class MetaData_MyEnum
{
public:
    constexpr MetaData_MyEnum() = delete;
    static constexpr const en::MetaDataEnum& GetMetaDataEnum() { return s_MetaDataEnum; }
    static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
    static constexpr en::MetaDataEnumValue s_MetaDataValues[] =
    {
        en::MetaDataEnumValue(en::Hash::CRC32("MyEnum::A"), "A", static_cast<en::U32>(MyEnum::A)),
        en::MetaDataEnumValue(en::Hash::CRC32("MyEnum::B"), "B", static_cast<en::U32>(MyEnum::B)),
        en::MetaDataEnumValue(en::Hash::CRC32("MyEnum::C"), "C", static_cast<en::U32>(MyEnum::C))
    };
    static constexpr en::MetaDataEnum s_MetaDataEnum = en::MetaDataEnum(en::Hash::CRC32("MyEnum"), "MyEnum", s_MetaDataValues, ENLIVE_ARRAY_SIZE(s_MetaDataValues));
    static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("MyEnum"), "MyEnum", ENLIVE_SIZE_OF(en::U32), ENLIVE_ALIGN_OF(en::U32), TypeTraits_Enum);
};

class TestClassA
{
public:
	friend class MetaData_TestClassA;
	static constexpr const en::MetaDataType& GetStaticMetaData();

	U32 GetA() const { return a; }
	I32 GetB() const { return b; }
	F32 GetC() const { return c; }
    MyEnum GetD() const { return d; }

	void SetA(U32 v) { a = v; }
	void SetB(I32 v) { b = v; }
	void SetC(F32 v) { c = v; }
    void SetD(MyEnum v) { d = v; }

private:
	U32 a;
	I32 b;
	F32 c;
    MyEnum d;
};

class MetaData_TestClassA
{
public:
	constexpr MetaData_TestClassA() = delete; 
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
    static constexpr en::MetaDataProperty s_MetaDataProperties[] =
    {
        en::MetaDataProperty(en::Hash::CRC32("TestClassA::a"), en::PrimitivesMetaData::GetType<en::U32>(), "a", ENLIVE_OFFSET_OF(en::TestClassA, a), en::PrimitivesMetaData::GetType<en::U32>().GetTraits()),
        en::MetaDataProperty(en::Hash::CRC32("TestClassA::b"), en::PrimitivesMetaData::GetType<en::I32>(), "b", ENLIVE_OFFSET_OF(en::TestClassA, b), en::PrimitivesMetaData::GetType<en::I32>().GetTraits()),
        en::MetaDataProperty(en::Hash::CRC32("TestClassA::c"), en::PrimitivesMetaData::GetType<en::F32>(), "c", ENLIVE_OFFSET_OF(en::TestClassA, c), en::PrimitivesMetaData::GetType<en::F32>().GetTraits()),
        en::MetaDataProperty(en::Hash::CRC32("TestClassA::d"), en::MetaData_MyEnum::GetMetaData(), "d", ENLIVE_OFFSET_OF(en::TestClassA, d), en::MetaData_MyEnum::GetMetaData().GetTraits(), en::Attribute_None, &en::MetaData_MyEnum::GetMetaDataEnum())
	};
	static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassA"), "TestClassA", ENLIVE_SIZE_OF(TestClassA), ENLIVE_ALIGN_OF(TestClassA), TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassA::GetStaticMetaData()
{
	return MetaData_TestClassA::GetMetaData();
}

} // namespace en