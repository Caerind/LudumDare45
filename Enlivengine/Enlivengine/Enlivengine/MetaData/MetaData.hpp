#pragma once

#include <Enlivengine/System/Hash.hpp>

namespace en
{

#define ENLIVE_ARRAY_SIZE(arr) static_cast<U32>(sizeof(arr)/sizeof(arr[0]))
#define ENLIVE_OFFSET_OF(type, member) static_cast<U32>(offsetof(type, member))
#define ENLIVE_SIZE_OF(type) static_cast<U32>(sizeof(type))

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

class MetaDataType;
class MetaData
{
public:
};

class MetaDataProperty
{
public:
	constexpr MetaDataProperty() = delete;
	constexpr MetaDataProperty(U32 propertyID, U32 typeID, std::string_view name, U32 offset) : mID(propertyID), mTypeID(typeID), mName(name), mOffset(offset) {}

	constexpr U32 GetID() const { return mID; }
	constexpr U32 GetTypeID() const { return mTypeID; }
	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetOffset() const { return mOffset; }

	constexpr bool operator==(const MetaDataProperty& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataProperty& other) const { return mID != other.mID; }

private:
	U32 mID;
	U32 mTypeID;
	std::string_view mName;
	U32 mOffset;
};

class MetaDataType
{
public:
	constexpr MetaDataType() = delete;
	constexpr MetaDataType(U32 id, std::string_view name, U32 size, const MetaDataProperty* properties = nullptr, U32 propertyCount = 0) : mID(id), mName(name), mSize(size), mProperties(properties), mPropertyCount(propertyCount) {}

	constexpr U32 GetID() const { return mID; }
	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetSize() const { return mSize; }

	constexpr U32 GetPropertyCount() const { return mPropertyCount; }
	constexpr const MetaDataProperty& GetPropertyByIndex(U32 index) const { return mProperties[index]; }

	constexpr bool operator==(const MetaDataType& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataType& other) const { return mID != other.mID; }

private:
	U32 mID;
    std::string_view mName;
	U32 mSize;
	const MetaDataProperty* mProperties;
	U32 mPropertyCount;
};

} // namespace en

#define ENLIVE_BEGIN_PROPERTIES(className) static constexpr en::MetaDataProperty className##_MetaDataProperties[] = {
#define ENLIVE_END_PROPERTIES() };
#define ENLIVE_TYPE(className) static constexpr en::MetaDataType className##_MetaData = en::MetaDataType(en::Hash::CRC32(#className), std::string_view(#className), sizeof(className), className##_MetaDataProperties, ENLIVE_ARRAY_SIZE(className##_MetaDataProperties));
