#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

class TestClassB
{
public:
    TestClassA a;
    F32 b;
    U32 c;

    static constexpr const en::MetaDataType& GetStaticMetaData();
    constexpr const en::MetaDataType& GetMetaData() { return GetStaticMetaData(); }
};

static constexpr en::MetaDataProperty TestClassB_MetaDataProperties[] =
{
    MetaDataProperty(Hash::CRC32("TestClassB::a"), TestClassA_MetaData, "a", ENLIVE_OFFSET_OF(TestClassB, a)),
    MetaDataProperty(Hash::CRC32("TestClassB::b"), F32_MetaData, "b", ENLIVE_OFFSET_OF(TestClassB, b)),
    MetaDataProperty(Hash::CRC32("TestClassB::c"), U32_MetaData, "c", ENLIVE_OFFSET_OF(TestClassB, c))
};
static constexpr en::MetaDataType TestClassB_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassB"), "TestClassB", ENLIVE_SIZE_OF(TestClassB), ENLIVE_ALIGN_OF(TestClassB), nullptr, TestClassB_MetaDataProperties, ENLIVE_ARRAY_SIZE(TestClassB_MetaDataProperties));

constexpr const en::MetaDataType& TestClassB::GetStaticMetaData()
{
    return TestClassB_MetaData;
}

} // namespace en