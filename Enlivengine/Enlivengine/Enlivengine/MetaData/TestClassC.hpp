#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

class TestClassC : public TestClassA
{
public:
    F32 b;
    U32 c;

    static constexpr const en::MetaDataType& GetStaticMetaData();
    constexpr const en::MetaDataType& GetMetaData() { return GetStaticMetaData(); }
};

static constexpr en::MetaDataProperty TestClassC_MetaDataProperties[] =
{
    MetaDataProperty(Hash::CRC32("TestClassC::b"), F32_MetaData, "b", ENLIVE_OFFSET_OF(TestClassC, b)),
    MetaDataProperty(Hash::CRC32("TestClassC::c"), U32_MetaData, "c", ENLIVE_OFFSET_OF(TestClassC, c))
};
static constexpr en::MetaDataType TestClassC_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassC"), "TestClassC", ENLIVE_SIZE_OF(TestClassC), ENLIVE_ALIGN_OF(TestClassC), &TestClassA_MetaData, TestClassC_MetaDataProperties, ENLIVE_ARRAY_SIZE(TestClassC_MetaDataProperties));

constexpr const en::MetaDataType& TestClassC::GetStaticMetaData()
{
    return TestClassC_MetaData;
}

} // namespace en