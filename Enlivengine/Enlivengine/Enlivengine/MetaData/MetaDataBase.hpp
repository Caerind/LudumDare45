#pragma once

#include <Enlivengine/System/Hash.hpp>

namespace en
{

#define ENLIVE_ARRAY_SIZE(arr) static_cast<en::U32>(sizeof(arr)/sizeof(arr[0]))
#define ENLIVE_SIZE_OF(type) static_cast<en::U32>(sizeof(type))
#define ENLIVE_ALIGN_OF(type) static_cast<en::U32>(alignof(type))
#define ENLIVE_OFFSET_OF(type, member) static_cast<en::U32>(offsetof(type, member))

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

	if (name.at(0) == 'c' && name.substr(0, 6) == "class ")
	{
		name.substr(6);
	}
	else if (name.at(0) == 's' && name.substr(0, 7) == "struct ")
	{
		return name.substr(7);
	}
	else
	{
		return name;
	}
}

template <typename T>
constexpr U32 GetTypeHash()
{
	return Hash::CRC32(GetTypeName<T>());
}

template <typename T, U32 Size>
class ConstexprVector
{
public:
    ConstexprVector() : mData(), mSize(0) {}

    constexpr const T& GetValue(U32 index) const { return mData[index]; }
    constexpr U32 GetSize() const { return mSize; }
   
    constexpr void Add(const T& value) { mData[mSize++] = std::move(value); }

private:
    T mData[Size];
    U32 mSize;
};

class MetaDataType;

static constexpr U32 Attribute_None = 0;
static constexpr U32 Attribute_Transient = (1 << 0);

static constexpr U32 TypeTraits_None = 0;
static constexpr U32 TypeTraits_Null = (1 << 0); // void, nullptr_t
static constexpr U32 TypeTraits_Primitive = (1 << 1); // int, float, char, bool, double, long, +/- unsigned, short, ...
static constexpr U32 TypeTraits_Pointer = (1 << 2);
static constexpr U32 TypeTraits_Reference = (1 << 3);
static constexpr U32 TypeTraits_Const = (1 << 4);
static constexpr U32 TypeTraits_Enum = (1 << 5);
static constexpr U32 TypeTraits_Union = (1 << 6);
static constexpr U32 TypeTraits_Class = (1 << 7);
static constexpr U32 TypeTraits_Array = (1 << 8);

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
	constexpr MetaDataProperty(U32 id, const MetaDataType& type, const char* name, U32 offset, U32 typeTraits, U32 attributes = Attribute_None, const MetaDataEnum* enumType = nullptr) : mID(id), mType(type), mName(name), mOffset(offset), mTypeTraits(typeTraits), mAttributes(attributes), mEnumType(enumType) {}

	constexpr U32 GetID() const { return mID; }
	constexpr const MetaDataType& GetType() const { return mType; }
	constexpr const char* GetName() const { return mName; }
	constexpr U32 GetOffset() const { return mOffset; }
	constexpr U32 GetTypeTraits() const { return mTypeTraits; }
    constexpr U32 GetAttributes() const { return mAttributes; }
    constexpr const MetaDataEnum* GetEnumType() const { return mEnumType; }

	constexpr bool operator==(const MetaDataProperty& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataProperty& other) const { return mID != other.mID; }

private:
	U32 mID;
    const MetaDataType& mType;
    const char* mName;
	U32 mOffset;
	U32 mTypeTraits;
	U32 mAttributes; 
    const MetaDataEnum* mEnumType;
};

class MetaDataType
{
public:
	constexpr MetaDataType() = delete;
	constexpr MetaDataType(U32 id, const char* name, U32 size, U32 alignment, U32 traits, const MetaDataType* parent = nullptr, const MetaDataProperty* properties = nullptr, U32 propertyCount = 0, U32 attributes = Attribute_None) : mID(id), mName(name), mSize(size), mAlignment(alignment), mTraits(traits), mAttributes(attributes), mSignature(id), mParent(parent), mProperties(properties), mPropertyCount(propertyCount) { mSignature = GenerateSignature(); }

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
	static constexpr en::U32 s_PrimitiveTypeCount = 4;
	static constexpr en::MetaDataType s_Void_MetaData = en::MetaDataType(en::Hash::CRC32("void"), "void", 0, 0, TypeTraits_Null, 0);
	static constexpr en::MetaDataType s_U32_MetaData = en::MetaDataType(en::Hash::CRC32("U32"), "U32", ENLIVE_SIZE_OF(en::U32), ENLIVE_ALIGN_OF(en::U32), TypeTraits_Primitive);
	static constexpr en::MetaDataType s_I32_MetaData = en::MetaDataType(en::Hash::CRC32("I32"), "I32", ENLIVE_SIZE_OF(en::I32), ENLIVE_ALIGN_OF(en::I32), TypeTraits_Primitive);
	static constexpr en::MetaDataType s_F32_MetaData = en::MetaDataType(en::Hash::CRC32("F32"), "F32", ENLIVE_SIZE_OF(en::F32), ENLIVE_ALIGN_OF(en::F32), TypeTraits_Primitive);
};

template <> constexpr const MetaDataType& PrimitivesMetaData::GetType<en::U32>() { return s_U32_MetaData; }
template <> constexpr const MetaDataType& PrimitivesMetaData::GetType<en::I32>() { return s_I32_MetaData; }
template <> constexpr const MetaDataType& PrimitivesMetaData::GetType<en::F32>() { return s_F32_MetaData; }

} // namespace en