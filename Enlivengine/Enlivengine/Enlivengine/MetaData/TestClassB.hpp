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

	const TestClassA& GetA() const { return a; }
	F32 GetB() const { return b; }
	U32 GetC() const { return c; }

	void SetA(const TestClassA& v) { a = v; }
	void SetB(F32 v) { b = v; }
	void SetC(U32 v) { c = v; }

private:
    TestClassA a;
    F32 b;
    U32 c;
};

class MetaData_TestClassB
{
public:
	constexpr MetaData_TestClassB() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::a"), en::TestClassA::GetStaticMetaData(), "a", ENLIVE_OFFSET_OF(en::TestClassB, a), en::TestClassA::GetStaticMetaData().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::b"), en::PrimitivesMetaData::GetType<en::F32>(), "b", ENLIVE_OFFSET_OF(en::TestClassB, b), en::PrimitivesMetaData::GetType<en::F32>().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassB::c"), en::PrimitivesMetaData::GetType<en::U32>(), "c", ENLIVE_OFFSET_OF(en::TestClassB, c), en::PrimitivesMetaData::GetType<en::U32>().GetTraits())
	};
	static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassB"), "TestClassB", ENLIVE_SIZE_OF(en::TestClassB), ENLIVE_ALIGN_OF(en::TestClassB), TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassB::GetStaticMetaData()
{
	return MetaData_TestClassB::GetMetaData();
}

} // namespace en