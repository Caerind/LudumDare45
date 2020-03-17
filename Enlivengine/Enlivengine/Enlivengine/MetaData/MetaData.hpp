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
	static constexpr const MetaDataType& GetType() { return GetTypeFromIndex(0); }
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

    template <typename T>
    static constexpr const MetaDataEnum& GetEnum() { return GetEnumFromIndex(0); }
    static constexpr U32 GetEnumCount() { return ENLIVE_ARRAY_SIZE(s_MetaDataEnums); }
    static constexpr const MetaDataEnum& GetEnumFromIndex(U32 index) { return *(s_MetaDataEnums[index]); }
    static constexpr const MetaDataEnum& GetEnumFromName(const char* name) { return GetEnumFromID(en::Hash::CRC32(name)); }
    static constexpr const MetaDataEnum& GetEnumFromID(U32 id)
    {
        constexpr U32 enumCount = GetEnumCount();
        for (U32 i = 0; i < enumCount; ++i)
        {
            if (s_MetaDataEnums[i]->GetID() == id)
            {
                return GetEnumFromIndex(i);
            }
        }
        return GetEnumFromIndex(0);
    }

private:
	static constexpr const MetaDataType* s_MetaDataTypes[] =
	{
		// Primitive types
		&en::PrimitivesMetaData::GetType<void>(),
		&en::PrimitivesMetaData::GetType<en::U32>(),
		&en::PrimitivesMetaData::GetType<en::I32>(),
		&en::PrimitivesMetaData::GetType<en::F32>(),
		&en::PrimitivesMetaData::GetType<bool>(),
		// Engine types
        &en::MetaData_MyEnum::GetMetaData(),
		&en::MetaData_TestClassA::GetMetaData(),
		&en::MetaData_TestClassB::GetMetaData(),
		&en::MetaData_TestClassC::GetMetaData(),
		&en::MetaData_TestArrayTemplate<en::U32>::GetMetaData(),
		&en::MetaData_TestArrayTemplate<en::F32>::GetMetaData()
		// User defined types
		// ...
	};
    static constexpr const MetaDataEnum* s_MetaDataEnums[] =
    {
        // Engine types
        &MetaData_MyEnum::GetMetaDataEnum(),
        // User defined types
        // ...
    };
};

// -- Types --
// Primitive types
template <> constexpr const MetaDataType& MetaData::GetType<void>() { return en::PrimitivesMetaData::GetType<void>(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::U32>() { return en::PrimitivesMetaData::GetType<en::U32>(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::I32>() { return en::PrimitivesMetaData::GetType<en::I32>(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::F32>() { return en::PrimitivesMetaData::GetType<en::F32>(); }
template <> constexpr const MetaDataType& MetaData::GetType<bool>() { return en::PrimitivesMetaData::GetType<bool>(); }
// Engine types
template <> constexpr const MetaDataType& MetaData::GetType<en::MyEnum>() { return en::MetaData_MyEnum::GetMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassA>() { return en::MetaData_TestClassA::GetMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassB>() { return en::MetaData_TestClassB::GetMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestClassC>() { return en::MetaData_TestClassC::GetMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestArrayTemplate<en::U32>>() { return en::MetaData_TestArrayTemplate<en::U32>::GetMetaData(); }
template <> constexpr const MetaDataType& MetaData::GetType<en::TestArrayTemplate<en::F32>>() { return en::MetaData_TestArrayTemplate<en::F32>::GetMetaData(); }
// User defined types
// ...

// -- Enums --
// Engine types
template <> constexpr const MetaDataEnum& MetaData::GetEnum<en::MyEnum>() { return en::MetaData_MyEnum::GetMetaDataEnum(); }
// User defined types
// ...

} // namespace en


