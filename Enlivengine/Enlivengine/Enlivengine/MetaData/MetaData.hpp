#pragma once

#include <Enlivengine/System/Hash.hpp>

namespace en
{

#define ENLIVE_ARRAY_SIZE(arr) static_cast<en::U32>(sizeof(arr)/sizeof(arr[0]))
#define ENLIVE_OFFSET_OF(type, member) static_cast<en::U32>(offsetof(type, member))
#define ENLIVE_SIZE_OF(type) static_cast<en::U32>(sizeof(type))
#define ENLIVE_ALIGN_OF(type) static_cast<en::U32>(alignof(type))

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

	// TODO : Is this really needed ?
	if (name.substr(0, 6) == "class ")
	{
		return name.substr(6);
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

class MetaDataProperty
{
public:
	constexpr MetaDataProperty() = delete;
	constexpr MetaDataProperty(U32 id, const MetaDataType& type, const char* name, U32 offset) : mID(id), mType(type), mName(name), mOffset(offset) {}

	constexpr U32 GetID() const { return mID; }
	constexpr const MetaDataType& GetType() const { return mType; }
	constexpr const char* GetName() const { return mName; }
	constexpr U32 GetOffset() const { return mOffset; }

	constexpr bool operator==(const MetaDataProperty& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataProperty& other) const { return mID != other.mID; }

private:
	U32 mID;
    const MetaDataType& mType;
    const char* mName;
	U32 mOffset;
};

class MetaDataType
{
public:
	constexpr MetaDataType() = delete;
	constexpr MetaDataType(U32 id, const char* name, U32 size, U32 alignment, const MetaDataType* parent = nullptr, const MetaDataProperty* properties = nullptr, U32 propertyCount = 0) : mID(id), mName(name), mSize(size), mAlignment(alignment), mParent(parent), mProperties(properties), mPropertyCount(propertyCount) {}

	constexpr U32 GetID() const { return mID; }
	constexpr const char* GetName() const { return mName; }
	constexpr U32 GetSize() const { return mSize; }
    constexpr U32 GetAlignment() const { return mAlignment; }

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
    const MetaDataType* mParent;
	const MetaDataProperty* mProperties;
	U32 mPropertyCount;
};

} // namespace en


static constexpr en::MetaDataType int_MetaData = en::MetaDataType(en::Hash::CRC32("int"), "int", ENLIVE_SIZE_OF(int), ENLIVE_ALIGN_OF(int));
static constexpr en::MetaDataType float_MetaData = en::MetaDataType(en::Hash::CRC32("float"), "float", ENLIVE_SIZE_OF(float), ENLIVE_ALIGN_OF(float));
static constexpr en::MetaDataType char_MetaData = en::MetaDataType(en::Hash::CRC32("char"), "char", ENLIVE_SIZE_OF(char), ENLIVE_ALIGN_OF(char));
static constexpr en::MetaDataType U32_MetaData = en::MetaDataType(en::Hash::CRC32("U32"), "U32", ENLIVE_SIZE_OF(en::U32), ENLIVE_ALIGN_OF(en::U32));
static constexpr en::MetaDataType I32_MetaData = en::MetaDataType(en::Hash::CRC32("I32"), "I32", ENLIVE_SIZE_OF(en::I32), ENLIVE_ALIGN_OF(en::I32));
static constexpr en::MetaDataType F32_MetaData = en::MetaDataType(en::Hash::CRC32("F32"), "F32", ENLIVE_SIZE_OF(en::F32), ENLIVE_ALIGN_OF(en::F32));

// Don't use that yet, need to be more stable/polished
//#define ENLIVE_BEGIN_PROPERTIES(className) static constexpr en::MetaDataProperty className##_MetaDataProperties[] = {
//#define ENLIVE_END_PROPERTIES() };
//#define ENLIVE_TYPE(className) static constexpr en::MetaDataType className##_MetaData = en::MetaDataType(en::Hash::CRC32(#className), std::string_view(#className), sizeof(className), className##_MetaDataProperties, ENLIVE_ARRAY_SIZE(className##_MetaDataProperties));
