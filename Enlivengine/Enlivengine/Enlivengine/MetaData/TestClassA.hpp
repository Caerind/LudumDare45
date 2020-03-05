#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

class TestClassA
{
public:
	int a;
	float b;

	static constexpr const en::MetaDataType& GetStaticMetaData();
};

static constexpr en::MetaDataType int_MetaData = en::MetaDataType(en::Hash::CRC32("int"), std::string_view("int"), ENLIVE_SIZE_OF(int));
static constexpr en::MetaDataType float_MetaData = en::MetaDataType(en::Hash::CRC32("float"), std::string_view("float"), ENLIVE_SIZE_OF(float));

static constexpr en::MetaDataProperty TestClassA_MetaDataProperties[] =
{
	MetaDataProperty(Hash::CRC32("TestClassA::a"), Hash::CRC32("int"), "a", ENLIVE_OFFSET_OF(TestClassA, a)),
	MetaDataProperty(Hash::CRC32("TestClassA::b"), Hash::CRC32("float"), "b", ENLIVE_OFFSET_OF(TestClassA, b))
};
static constexpr en::MetaDataType TestClassA_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassA"), std::string_view("TestClassA"), ENLIVE_SIZE_OF(TestClassA), TestClassA_MetaDataProperties, ENLIVE_ARRAY_SIZE(TestClassA_MetaDataProperties));

constexpr const en::MetaDataType& TestClassA::GetStaticMetaData()
{
	return TestClassA_MetaData;
}

} // namespace en