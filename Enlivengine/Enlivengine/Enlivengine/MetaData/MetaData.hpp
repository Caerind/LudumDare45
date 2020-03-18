#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>

#ifdef ENLIVE_ENABLE_METADATA

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
		&en::PrimitivesMetaData::GetType<bool>(),
		&en::PrimitivesMetaData::GetType<en::I8>(),
		&en::PrimitivesMetaData::GetType<en::U8>(),
		&en::PrimitivesMetaData::GetType<en::I16>(),
		&en::PrimitivesMetaData::GetType<en::U16>(),
		&en::PrimitivesMetaData::GetType<en::I32>(),
		&en::PrimitivesMetaData::GetType<en::U32>(),
		&en::PrimitivesMetaData::GetType<en::I64>(),
		&en::PrimitivesMetaData::GetType<en::U64>(),
		&en::PrimitivesMetaData::GetType<en::F32>(),
		&en::PrimitivesMetaData::GetType<en::F64>(),
		// Engine types
        &en::MetaData_MyEnum::GetMetaData(),
		&en::MetaData_TestClassA::GetMetaData(),
		&en::MetaData_TestClassB::GetMetaData(),
		// User defined types
		&MetaData_TestClassC::GetMetaData(),
		&MetaData_TestArrayTemplate<en::U32>::GetMetaData(),
		&MetaData_TestArrayTemplate<en::F32>::GetMetaData()
	};
    static constexpr const MetaDataEnum* s_MetaDataEnums[] =
    {
        // Engine types
        &MetaData_MyEnum::GetMetaDataEnum(),
        // User defined types
        // ...
    };
};

#define ENLIVE_METADATA_DEF_TYPE(type, metaDataType) template <> constexpr const MetaDataType& MetaData::GetType<type>() { return metaDataType; }
#define ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(type) ENLIVE_METADATA_DEF_TYPE(type, en::PrimitivesMetaData::GetType<type>())
#define ENLIVE_METADATA_DEF_TYPE_EN(type) ENLIVE_METADATA_DEF_TYPE(en::##type, en::MetaData_##type##::GetMetaData())
#define ENLIVE_METADATA_DEF_ENUM(type, metaDataType) template <> constexpr const MetaDataEnum& MetaData::GetEnum<type>() { return metaDataType; }
#define ENLIVE_METADATA_DEF_ENUM_EN(type) ENLIVE_METADATA_DEF_ENUM(en::##type, en::MetaData_##type##::GetMetaDataEnum())

// --------------------------------
// --         Types              --
// --------------------------------
//  Primitive types
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(void);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(bool);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::I8);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::U8);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::I16);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::U16);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::I32);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::U32);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::I64);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::U64);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::F32);
ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE(en::F64);
//  Engine types
ENLIVE_METADATA_DEF_TYPE_EN(MyEnum);
ENLIVE_METADATA_DEF_TYPE_EN(TestClassA);
ENLIVE_METADATA_DEF_TYPE_EN(TestClassB);
//  User defined types
ENLIVE_METADATA_DEF_TYPE(TestClassC, MetaData_TestClassC::GetMetaData());
ENLIVE_METADATA_DEF_TYPE(TestArrayTemplate<en::U32>, MetaData_TestArrayTemplate<en::U32>::GetMetaData());
ENLIVE_METADATA_DEF_TYPE(TestArrayTemplate<en::F32>, MetaData_TestArrayTemplate<en::F32>::GetMetaData());

// --------------------------------
// --         Enums              --
// --------------------------------
//  Engine types
ENLIVE_METADATA_DEF_ENUM_EN(MyEnum);
//  User defined types
// ...


#undef ENLIVE_METADATA_DEF_TYPE
#undef ENLIVE_METADATA_DEF_TYPE_PRIMITIVE_TYPE
#undef ENLIVE_METADATA_DEF_TYPE_EN
#undef ENLIVE_METADATA_DEF_ENUM
#undef ENLIVE_METADATA_DEF_ENUM_EN

} // namespace en

#endif // ENLIVE_ENABLE_METADATA

