#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

class TestClassB
{
public:
	friend class MetaData_TestClassB;
	static constexpr const en::MetaDataType& GetStaticMetaData();

	TestClassA* GetA() const { return a; }
	F32 GetB() const { return b; }
    U32 GetC() const { return c; }
    F32* GetD() const { return d; }
    MyEnum* GetE() const { return e; }

	void SetA(TestClassA* v) { a = v; }
	void SetB(F32 v) { b = v; }
	void SetC(U32 v) { c = v; }
	void SetD(F32* v) { d = v; }
	void SetE(MyEnum* v) { e = v; }

private:
    TestClassA* a;
    F32 b;
    U32 c;
    F32* d;
    MyEnum* e;
};

class MetaData_TestClassB
{
public:
	constexpr MetaData_TestClassB() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::a"), en::MetaData_TestClassA::GetMetaData(), "a", ENLIVE_OFFSET_OF(en::TestClassB, a), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, a), en::MetaData_TestClassA::GetMetaData().GetTraits() | en::TypeTraits_Pointer),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::b"), en::PrimitivesMetaData::GetType<en::F32>(), "b", ENLIVE_OFFSET_OF(en::TestClassB, b), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, b), en::PrimitivesMetaData::GetType<en::F32>().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::c"), en::PrimitivesMetaData::GetType<en::U32>(), "c", ENLIVE_OFFSET_OF(en::TestClassB, c), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, c), en::PrimitivesMetaData::GetType<en::U32>().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::d"), en::PrimitivesMetaData::GetType<en::F32>(), "d", ENLIVE_OFFSET_OF(en::TestClassB, d), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, d), en::PrimitivesMetaData::GetType<en::F32>().GetTraits() | en::TypeTraits_Pointer),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::e"), en::MetaData_MyEnum::GetMetaData(), "e", ENLIVE_OFFSET_OF(en::TestClassB, e), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, e), en::MetaData_MyEnum::GetMetaData().GetTraits() | en::TypeTraits_Pointer)
	};
	static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassB"), "TestClassB", ENLIVE_SIZE_OF(en::TestClassB), ENLIVE_ALIGN_OF(en::TestClassB), TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassB::GetStaticMetaData()
{
	return en::MetaData_TestClassB::GetMetaData();
}

} // namespace en