#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <array>

#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/MetaData/TestClassB.hpp>
#include <Enlivengine/MetaData/TestClassC.hpp>

namespace en
{

class MetaData
{
public:
	constexpr MetaData() = delete;

	template <typename T>
	static constexpr const MetaDataType& GetType() { return s_MetaDataTypes[0]; }

	static constexpr U32 GetTypeCount() { return ENLIVE_ARRAY_SIZE(s_MetaDataTypes); }
	static constexpr const MetaDataType& GetTypeFromIndex(U32 index) { return *(s_MetaDataTypes[index]); }
	static constexpr const MetaDataType& GetTypeFromName(const char* name) { return GetTypeFromID(en::Hash::CRC32(name)); }
	static constexpr const MetaDataType& GetTypeFromID(U32 id)
	{
		constexpr U32 typeCount = GetTypeCount();
		for (U32 i = 0; i < typeCount; ++i)
		{
			if (s_MetaDataTypes[i]->GetID() == id)
			{
				return GetTypeFromIndex(i);
			}
		}
		return GetTypeFromIndex(0);
	}

private:
	static constexpr const MetaDataType* s_MetaDataTypes[] =
	{
		// Primitive types
		&PrimitivesMetaData::GetType<void>(),
		&PrimitivesMetaData::GetType<en::U32>(),
		&PrimitivesMetaData::GetType<en::I32>(),
		&PrimitivesMetaData::GetType<en::F32>(),
		// Engine types
		&TestClassA::GetStaticMetaData(),
		&TestClassB::GetStaticMetaData(),
		&TestClassC::GetStaticMetaData()
		// User defined types
		// ...
	};
};

// Primitive types
template <> constexpr const MetaDataType& MetaData::GetType<en::U32>() { return PrimitivesMetaData::GetType<en::U32>(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::I32>() { return PrimitivesMetaData::GetType<en::I32>(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::F32>() { return PrimitivesMetaData::GetType<en::F32>(); }
// Engine types
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassA>() { return TestClassA::GetStaticMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassB>() { return TestClassB::GetStaticMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassC>() { return TestClassC::GetStaticMetaData(); }
// User defined types
// ...

} // namespace en


