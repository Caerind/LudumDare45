#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>

namespace en
{

class TestClassA
{
public:
	friend class MetaData_TestClassA;
	static constexpr const en::MetaDataType& GetStaticMetaData();
	virtual const en::MetaDataType& GetMetaData() { return GetStaticMetaData(); }

	U32 GetA() const { return a; }
	I32 GetB() const { return b; }
	F32 GetC() const { return c; }

private:
	U32 a;
	I32 b;
	F32 c;
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
		en::MetaDataProperty(en::Hash::CRC32("TestClassA::c"), en::PrimitivesMetaData::GetType<en::F32>(), "c", ENLIVE_OFFSET_OF(en::TestClassA, c), en::PrimitivesMetaData::GetType<en::F32>().GetTraits())
	};
	static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassA"), "TestClassA", ENLIVE_SIZE_OF(TestClassA), ENLIVE_ALIGN_OF(TestClassA), TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassA::GetStaticMetaData()
{
	return MetaData_TestClassA::GetMetaData();
}

} // namespace en