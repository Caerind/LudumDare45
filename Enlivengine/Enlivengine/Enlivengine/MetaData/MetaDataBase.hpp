#pragma once

#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/System/PrimitiveTypes.hpp>

#ifdef ENLIVE_ENABLE_METADATA

#include <Enlivengine/System/Hash.hpp>

namespace en
{

// https://stackoverflow.com/a/56766138
template <typename T>
constexpr std::string_view GetTypeName()
{
	std::string_view name, prefix, suffix;
#ifdef ENLIVE_COMPILER_CLANG
	name = __PRETTY_FUNCTION__;
	prefix = "std::string_view en::GetTypeName() [T = ";
	suffix = "]";
#elif defined(ENLIVE_COMPILER_GNUC)
	name = __PRETTY_FUNCTION__;
	prefix = "constexpr std::string_view en::GetTypeName() [with T = ";
	suffix = "; std::string_view = std::basic_string_view<char>]";
#elif defined(ENLIVE_COMPILER_MSVC)
	name = __FUNCSIG__;
	prefix = "class std::basic_string_view<char,struct std::char_traits<char> > __cdecl en::GetTypeName<";
	suffix = ">(void)";
#else
	// Undefined
#endif
	name.remove_prefix(prefix.size());
	name.remove_suffix(suffix.size());

	if (name.size() > 0)
	{
		if (name[0] == 'c' && name.substr(0, 6) == "class ")
		{
			name.substr(6);
		}
		else if (name[0] == 's' && name.substr(0, 7) == "struct ")
		{
			return name.substr(7);
		}
	}
	return name;
}

template <typename T>
constexpr U32 GetTypeHash()
{
	return Hash::CRC32(GetTypeName<T>());
}


class MetaDataType;

static constexpr U32 Attribute_None = 0;
static constexpr U32 Attribute_Transient = (1 << 0);

static constexpr U32 TypeTraits_None = 0;
static constexpr U32 TypeTraits_Null = (1 << 0); // void, nullptr_t
static constexpr U32 TypeTraits_Primitive = (1 << 1); // int, float, char, bool, double, long, +/- unsigned, short, ...
static constexpr U32 TypeTraits_Pointer = (1 << 2); // *
static constexpr U32 TypeTraits_Reference = (1 << 3); // &
static constexpr U32 TypeTraits_Enum = (1 << 4); // enum (class)
static constexpr U32 TypeTraits_Class = (1 << 5); // class, struct
static constexpr U32 TypeTraits_Array = (1 << 6); // T myArray[x]
// TODO : static constexpr U32 TypeTraits_ArrayPtr = (1 << 7); // T (*myArray)[]
// TODO : static constexpr U32 TypeTraits_Const = (1 << 8); // const
// TODO : static constexpr U32 TypeTraits_Union = (1 << 9); // union

class MetaDataEnumValue
{
public:
    constexpr MetaDataEnumValue() = delete;
    constexpr MetaDataEnumValue(U32 id, const char* name, U32 value, U32 attributes = Attribute_None) : mID(id), mName(name), mValue(value), mAttributes(attributes) {}

    constexpr U32 GetID() const { return mID; }
    constexpr const char* GetName() const { return mName; }
    constexpr U32 GetValue() const { return mValue; }
    constexpr U32 GetAttributes() const { return mAttributes; }

    constexpr bool operator==(const MetaDataEnumValue& other) const { return mID == other.mID; }
    constexpr bool operator!=(const MetaDataEnumValue& other) const { return mID != other.mID; }

private:
    U32 mID;
    const char* mName;
    U32 mValue;
    U32 mAttributes;
};

class MetaDataEnum
{
public:
    constexpr MetaDataEnum() = delete;
    constexpr MetaDataEnum(U32 id, const char* name, const MetaDataEnumValue* values, U32 valueCount, U32 attributes = Attribute_None) : mID(id), mName(name), mValues(values), mValueCount(valueCount), mAttributes(attributes) {}

    constexpr U32 GetID() const { return mID; }
    constexpr const char* GetName() const { return mName; }
    constexpr U32 GetValueCount() const { return mValueCount; }
    constexpr const MetaDataEnumValue& GetValueByIndex(U32 index) const { return mValues[index]; }
    constexpr U32 GetAttributes() const { return mAttributes; }

    constexpr const MetaDataEnumValue& GetValueByValue(U32 value) const 
    { 
        for (U32 i = 0; i < mValueCount; ++i)
        {
            if (mValues[i].GetValue() == value)
            {
                return mValues[i];
            }
        }
        return mValues[0];
    }

    constexpr bool operator==(const MetaDataEnum& other) const { return mID == other.mID; }
    constexpr bool operator!=(const MetaDataEnum& other) const { return mID != other.mID; }

private:
    U32 mID;
    const char* mName;
    const MetaDataEnumValue* mValues;
    U32 mValueCount;
    U32 mAttributes;
};

class MetaDataProperty
{
public:
	constexpr MetaDataProperty() = delete;
	constexpr MetaDataProperty(U32 id, const MetaDataType& type, const char* name, U32 offset, U32 size, U32 typeTraits, U32 elementCount = 1, U32 attributes = Attribute_None) : mID(id), mType(type), mName(name), mOffset(offset), mSize(size), mTypeTraits(typeTraits), mElementCount(elementCount), mAttributes(attributes) {}

	constexpr U32 GetID() const { return mID; }
	constexpr const MetaDataType& GetType() const { return mType; }
	constexpr const char* GetName() const { return mName; }
	constexpr U32 GetOffset() const { return mOffset; }
    constexpr U32 GetSize() const { return mSize; }
	constexpr U32 GetTypeTraits() const { return mTypeTraits; }
	constexpr U32 GetElementCount() const { return mElementCount; }
	constexpr U32 GetElementSize() const { return mSize / mElementCount; }
    constexpr U32 GetAttributes() const { return mAttributes; }

	constexpr bool operator==(const MetaDataProperty& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataProperty& other) const { return mID != other.mID; }

private:
	U32 mID;
    const MetaDataType& mType;
    const char* mName;
	U32 mOffset;
    U32 mSize;
	U32 mTypeTraits;
	U32 mElementCount;
	U32 mAttributes;
};

class MetaDataType
{
public:
	constexpr MetaDataType() = delete;
	constexpr MetaDataType(U32 id, const char* name, U32 size, U32 alignment, U32 traits, const MetaDataType* parent = nullptr, const MetaDataProperty* properties = nullptr, U32 propertyCount = 0, U32 attributes = Attribute_None, const MetaDataEnum* enumType = nullptr, const MetaDataType* const* templateTypes = nullptr, U32 templateTypeCount = 0) : mID(id), mName(name), mSize(size), mAlignment(alignment), mTraits(traits), mAttributes(attributes), mSignature(id), mParent(parent), mProperties(properties), mPropertyCount(propertyCount), mEnumType(enumType), mTemplateTypes(templateTypes), mTemplateTypeCount(templateTypeCount) { mSignature = GenerateSignature(); }

	constexpr U32 GetID() const { return mID; }
	constexpr const char* GetName() const { return mName; }
	constexpr U32 GetSize() const { return mSize; }
	constexpr U32 GetAlignment() const { return mAlignment; }
	constexpr U32 GetTraits() const { return mTraits; }
	constexpr U32 GetAttributes() const { return mAttributes; }
	constexpr U32 GetSignature() const { return mSignature; }

    constexpr const MetaDataType* GetParent() const { return mParent; }

	constexpr U32 GetPropertyCount() const { return mPropertyCount; }
	constexpr const MetaDataProperty& GetPropertyByIndex(U32 index) const { return mProperties[index]; }

	constexpr const MetaDataEnum* GetEnumType() const { return mEnumType; }

	constexpr U32 GetTemplateTypeCount() const { return mTemplateTypeCount; }
	constexpr const MetaDataType& GetTemplateTypeByIndex(U32 index) const { return *mTemplateTypes[index]; }

	constexpr bool operator==(const MetaDataType& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataType& other) const { return mID != other.mID; }

private:
	U32 mID;
    const char* mName;
	U32 mSize;
	U32 mAlignment;
	U32 mTraits;
	U32 mAttributes;
	U32 mSignature;
    const MetaDataType* mParent;
	const MetaDataProperty* mProperties;
    U32 mPropertyCount;
	const MetaDataEnum* mEnumType;
	const MetaDataType* const* mTemplateTypes;
	U32 mTemplateTypeCount;

private:
	constexpr U32 GenerateSignature() const
	{
		U32 signature = mSize;
		signature = Hash::CombineHash(signature, mAlignment);
		signature = Hash::CombineHash(signature, mTraits);
		signature = Hash::CombineHash(signature, mAttributes);
		if (mParent != nullptr)
		{
			signature = Hash::CombineHash(signature, mParent->GetSignature());
		}
		if (mProperties != nullptr && mPropertyCount > 0)
		{
			for (U32 i = 0; i < mPropertyCount; ++i)
			{
				signature = Hash::CombineHash(signature, mProperties[i].GetType().GetSize());
				signature = Hash::CombineHash(signature, mProperties[i].GetOffset());
				signature = Hash::CombineHash(signature, mProperties[i].GetTypeTraits());
				signature = Hash::CombineHash(signature, mProperties[i].GetAttributes());
			}
        }
		return signature;
	}
};

class PrimitivesMetaData
{
public:
	constexpr PrimitivesMetaData() = delete;

	template <typename T>
	static constexpr const MetaDataType& GetType()
	{
		return s_Void_MetaData;
	}

private:
	static constexpr en::U32 s_PrimitiveTypeCount = 5;
	static constexpr en::MetaDataType s_Void_MetaData = en::MetaDataType(en::Hash::CRC32("void"), "void", 0, 0, TypeTraits_Null);
#define PRIMITIVE_TYPE_DECL(name, fullName) static constexpr en::MetaDataType s_##name##_MetaData = en::MetaDataType(en::Hash::CRC32(#name), #name, ENLIVE_SIZE_OF(##fullName), ENLIVE_ALIGN_OF(##fullName), TypeTraits_Primitive);
	PRIMITIVE_TYPE_DECL(bool, bool);
	PRIMITIVE_TYPE_DECL(I8, en::I8);
	PRIMITIVE_TYPE_DECL(U8, en::U8);
	PRIMITIVE_TYPE_DECL(I16, en::I16);
	PRIMITIVE_TYPE_DECL(U16, en::U16);
	PRIMITIVE_TYPE_DECL(I32, en::I32);
	PRIMITIVE_TYPE_DECL(U32, en::U32);
	PRIMITIVE_TYPE_DECL(I64, en::I64);
	PRIMITIVE_TYPE_DECL(U64, en::U64);
	PRIMITIVE_TYPE_DECL(F32, en::F32);
	PRIMITIVE_TYPE_DECL(F64, en::F64);
#undef PRIMITIVE_TYPE_DECL
};

#define PRIMITIVE_TYPE_DEF(name, fullName) template <> constexpr const MetaDataType& PrimitivesMetaData::GetType<##fullName>() { return s_##name##_MetaData; }
PRIMITIVE_TYPE_DEF(bool, bool);
PRIMITIVE_TYPE_DEF(I8, en::I8);
PRIMITIVE_TYPE_DEF(U8, en::U8);
PRIMITIVE_TYPE_DEF(I16, en::I16);
PRIMITIVE_TYPE_DEF(U16, en::U16);
PRIMITIVE_TYPE_DEF(I32, en::I32);
PRIMITIVE_TYPE_DEF(U32, en::U32);
PRIMITIVE_TYPE_DEF(I64, en::I64);
PRIMITIVE_TYPE_DEF(U64, en::U64);
PRIMITIVE_TYPE_DEF(F32, en::F32);
PRIMITIVE_TYPE_DEF(F64, en::F64);
#undef PRIMITIVE_TYPE_DEF

// Enum macros
#define ENLIVE_META_ENUM(name) ENLIVE_META_ENUM_AS(name, en::U32)
#define ENLIVE_META_ENUM_AS(name, type) enum class name : type
#define ENLIVE_META_ENUM_DEF(name) \
	class MetaData_##name \
	{ \
	public: \
		constexpr MetaData_##name() = delete; \
		static constexpr const en::MetaDataEnum& GetMetaDataEnum() { return s_MetaDataEnum; } \
		static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; } \
	private: \
		static constexpr const en::MetaDataEnumValue s_MetaDataValues[] = { 

#define ENLIVE_META_ENUM_VALUE(enumName, valueName) ENLIVE_META_ENUM_VALUE_EX(enumName, valueName, en::U32)
#define ENLIVE_META_ENUM_VALUE_EX(enumName, valueName, type) en::MetaDataEnumValue(en::Hash::CRC32(#enumName"::"#valueName), #valueName, static_cast<en::U32>(##enumName::##valueName))

#define ENLIVE_META_ENUM_DEF_END(name) ENLIVE_META_ENUM_DEF_END_EX_ATTR(name, en::U32, en::Attribute_None)
#define ENLIVE_META_ENUM_DEF_END_ATTR(name, attributes) ENLIVE_META_ENUM_DEF_END_EX_ATTR(name, en::U32, attributes)
#define ENLIVE_META_ENUM_DEF_END_EX(name, type) ENLIVE_META_ENUM_DEF_END_EX_ATTR(name, type, en::Attribute_None)
#define ENLIVE_META_ENUM_DEF_END_EX_ATTR(name, type, attributes) \
		};\
		static constexpr const en::MetaDataEnum s_MetaDataEnum = en::MetaDataEnum(en::Hash::CRC32(#name), #name, s_MetaDataValues, ENLIVE_ARRAY_SIZE(s_MetaDataValues)); \
		static constexpr const en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32(#name), #name, ENLIVE_SIZE_OF(type), ENLIVE_ALIGN_OF(type), en::TypeTraits_Enum, nullptr, nullptr, 0, attributes, &s_MetaDataEnum); \
	};

// Class macros
#define ENLIVE_META_CLASS_DECL(name) \
	public: \
		friend class MetaData_##name; \
		static constexpr const en::MetaDataType& GetStaticMetaData();

#define ENLIVE_META_CLASS_PROPERTY(className, name, metaDataType) ENLIVE_META_CLASS_PROPERTY_EX_ATTR(className, name, metaDataType, en::TypeTraits_None, en::Attribute_None)
#define ENLIVE_META_CLASS_PROPERTY_ATTR(className, name, metaDataType, attributes) ENLIVE_META_CLASS_PROPERTY_EX_ATTR(className, name, metaDataType, en::TypeTraits_None, attributes)
#define ENLIVE_META_CLASS_PROPERTY_EX(className, name, metaDataType, traits) ENLIVE_META_CLASS_PROPERTY_EX_ATTR(className, name, metaDataType, traits, en::Attribute_None)
#define ENLIVE_META_CLASS_PROPERTY_EX_ATTR(className, name, metaDataType, traits, attributes) en::MetaDataProperty(en::Hash::CRC32(#className"::"#name), metaDataType, #name, ENLIVE_OFFSET_OF(className, name), ENLIVE_SIZE_OF_MEMBER(className, name), metaDataType.GetTraits() | traits, attributes)
#define ENLIVE_META_CLASS_PROPERTY_ARRAY(className, name, metaDataType, elementCount) ENLIVE_META_CLASS_PROPERTY_EX_ARRAY_ATTR(className, name, metaDataType, en::TypeTraits_None, elementCount, en::Attribute_None)
#define ENLIVE_META_CLASS_PROPERTY_ARRAY_ATTR(className, name, metaDataType, elementCount, attributes) ENLIVE_META_CLASS_PROPERTY_EX_ARRAY_ATTR(className, name, metaDataType, en::TypeTraits_None, elementCount, attributes)
#define ENLIVE_META_CLASS_PROPERTY_EX_ARRAY(className, name, metaDataType, traits, elementCount) ENLIVE_META_CLASS_PROPERTY_EX_ARRAY_ATTR(className, name, metaDataType, traits, elementCount, en::Attribute_None)
#define ENLIVE_META_CLASS_PROPERTY_EX_ARRAY_ATTR(className, name, metaDataType, traits, elementCount, attributes) en::MetaDataProperty(en::Hash::CRC32(#className"::"#name), metaDataType, #name, ENLIVE_OFFSET_OF(className, name), ENLIVE_SIZE_OF_MEMBER(className, name), metaDataType.GetTraits() | en::TypeTraits_Array | traits, elementCount, attributes)

#define ENLIVE_META_CLASS_DEF(name) \
	class MetaData_##name \
	{ \
	public: \
		constexpr MetaData_##name() = delete; \
		static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; } \
	private: \
		static constexpr const en::MetaDataProperty s_MetaDataProperties[] = {

#define ENLIVE_META_CLASS_DEF_END(name, fullName) ENLIVE_META_CLASS_DEF_END_EX(name, fullName, nullptr)
#define ENLIVE_META_CLASS_DEF_END_EX(name, fullName, parentMetaDataTypePtr) \
		}; \
		static constexpr const en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32(#name), #name, ENLIVE_SIZE_OF(name), ENLIVE_ALIGN_OF(name), en::TypeTraits_Class, parentMetaDataTypePtr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties)); \
	}; \
	constexpr const en::MetaDataType& name::GetStaticMetaData() \
	{ \
		return MetaData_##name::GetMetaData(); \
	}

#define ENLIVE_METADATA_ONLY(code) code
#define ENLIVE_METADATA_COMMA() ,

} // namespace en

#else // ENLIVE_ENABLE_METADATA

#define ENLIVE_META_ENUM(name) enum class name : en::U32
#define ENLIVE_META_ENUM_AS(name, type) enum class name : type
#define ENLIVE_META_ENUM_DEF(name)
#define ENLIVE_META_ENUM_VALUE(enumName, valueName)
#define ENLIVE_META_ENUM_VALUE_EX(enumName, valueName, type)
#define ENLIVE_META_ENUM_DEF_END(name)
#define ENLIVE_META_ENUM_DEF_END_ATTR(name, attributes)
#define ENLIVE_META_ENUM_DEF_END_EX(name, type)
#define ENLIVE_META_ENUM_DEF_END_EX_ATTR(name, type, attributes)
#define ENLIVE_META_CLASS_DECL(name)
#define ENLIVE_META_CLASS_PROPERTY(className, name, metaDataType)
#define ENLIVE_META_CLASS_PROPERTY_ATTR(className, name, metaDataType, attributes)
#define ENLIVE_META_CLASS_PROPERTY_EX(className, name, metaDataType, traits)
#define ENLIVE_META_CLASS_PROPERTY_EX_ATTR(className, name, metaDataType, traits, attributes)
#define ENLIVE_META_CLASS_PROPERTY_ARRAY(className, name, metaDataType, elementCount)
#define ENLIVE_META_CLASS_PROPERTY_ARRAY_ATTR(className, name, metaDataType, elementCount, attributes)
#define ENLIVE_META_CLASS_PROPERTY_EX_ARRAY(className, name, metaDataType, traits, elementCount)
#define ENLIVE_META_CLASS_PROPERTY_EX_ARRAY_ATTR(className, name, metaDataType, traits, elementCount, attributes)
#define ENLIVE_META_CLASS_DEF(name)
#define ENLIVE_META_CLASS_DEF_END(name, fullName)
#define ENLIVE_META_CLASS_DEF_END_EX(name, fullName, parentMetaDataTypePtr)

#define ENLIVE_METADATA_ONLY(code)
#define ENLIVE_METADATA_COMMA()

#endif // ENLIVE_ENABLE_METADATA