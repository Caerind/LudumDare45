#pragma once

#include <array>
#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

class TestClassB
{
public:
	float c;
	int d;

	// Type
public:
	static constexpr U32 ClassID = MetaDataTypes::GetType<TestClassB>().GetID();
	static constexpr std::string_view ClassName = MetaDataTypes::GetType<TestClassB>().GetName();
	static constexpr const MetaDataType& GetMetaDataType() { return MetaDataTypes::GetType(ClassID); }

	// Property list
public:
	static constexpr U32 GetPropertieCount() { return kPropertyCount; }
	static constexpr const MetaDataProperty& GetPropertyByIndex(U32 index) { return kProperties[index]; }
private:
	static constexpr U32 kPropertyCount = 2;
	static constexpr std::array<MetaDataProperty, kPropertyCount> kProperties = {
		MetaDataProperty(MetaDataTypes::GetType<float>(), "c", Hash::CRC32("c"), {}),
		MetaDataProperty(MetaDataTypes::GetType<int>(), "d", Hash::CRC32("d"), {})
	};
};

} // namespace en