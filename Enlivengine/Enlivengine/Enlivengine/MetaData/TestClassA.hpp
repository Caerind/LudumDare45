#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

class TestClassA
{
public:
	U32 a;
	I32 b;
	F32 c;

    static constexpr const en::MetaDataType& GetStaticMetaData();
    constexpr const en::MetaDataType& GetMetaData() { return GetStaticMetaData(); }
};

static constexpr en::MetaDataProperty TestClassA_MetaDataProperties[] =
{
	MetaDataProperty(Hash::CRC32("TestClassA::a"), U32_MetaData, "a", ENLIVE_OFFSET_OF(TestClassA, a)),
	MetaDataProperty(Hash::CRC32("TestClassA::b"), I32_MetaData, "b", ENLIVE_OFFSET_OF(TestClassA, b)),
	MetaDataProperty(Hash::CRC32("TestClassA::c"), F32_MetaData, "c", ENLIVE_OFFSET_OF(TestClassA, c))
};
static constexpr en::MetaDataType TestClassA_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassA"), "TestClassA", ENLIVE_SIZE_OF(TestClassA), ENLIVE_ALIGN_OF(TestClassA), nullptr, TestClassA_MetaDataProperties, ENLIVE_ARRAY_SIZE(TestClassA_MetaDataProperties));

constexpr const en::MetaDataType& TestClassA::GetStaticMetaData()
{
	return TestClassA_MetaData;
}

} // namespace en