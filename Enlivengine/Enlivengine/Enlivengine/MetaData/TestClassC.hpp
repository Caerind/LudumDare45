#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

class TestClassC : public TestClassA
{
public:
	friend class MetaData_TestClassC;
	static constexpr const en::MetaDataType& GetStaticMetaData();

	F32 GetB() const { return b; }
	U32 GetC() const { return c; }

	void SetB(F32 v) { b = v; }
	void SetC(U32 v) { c = v; }

private:
    F32 b;
    U32 c;
};

class MetaData_TestClassC
{
public:
	constexpr MetaData_TestClassC() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestClassC::b"), en::PrimitivesMetaData::GetType<en::F32>(), "b", ENLIVE_OFFSET_OF(en::TestClassC, b), en::PrimitivesMetaData::GetType<en::F32>().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassC::c"), en::PrimitivesMetaData::GetType<en::U32>(), "c", ENLIVE_OFFSET_OF(en::TestClassC, c), en::PrimitivesMetaData::GetType<en::U32>().GetTraits())
	};
	static constexpr en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassC"), "TestClassC", ENLIVE_SIZE_OF(en::TestClassC), ENLIVE_ALIGN_OF(en::TestClassC), TypeTraits_Class, &TestClassA::GetStaticMetaData(), s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassC::GetStaticMetaData()
{
	return MetaData_TestClassC::GetMetaData();
}

} // namespace en