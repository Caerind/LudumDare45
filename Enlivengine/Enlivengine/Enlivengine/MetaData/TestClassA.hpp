#pragma once

#include <array>
#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

class TestClassA
{
public:
	int a;
	float b;

	// Type
public:
	static constexpr U32 ClassID = MetaDataTypes::GetType<TestClassA>().GetID();
	static constexpr std::string_view ClassName = MetaDataTypes::GetType<TestClassA>().GetName();
	static constexpr const MetaDataType& GetMetaDataType() { return MetaDataTypes::GetType(ClassID); }

	// Property list
public:
	static constexpr U32 GetPropertieCount() { return kPropertyCount; }
	static constexpr const MetaDataProperty& GetPropertyByIndex(U32 index) { return kProperties[index]; }
private:
	static constexpr U32 kPropertyCount = 2;
	static constexpr std::array<MetaDataProperty, kPropertyCount> kProperties = {
		MetaDataProperty(MetaDataTypes::GetType<int>(), "a", Hash::CRC32("a"), {}),
		MetaDataProperty(MetaDataTypes::GetType<float>(), "b", Hash::CRC32("b"), {})
	};
};

} // namespace en