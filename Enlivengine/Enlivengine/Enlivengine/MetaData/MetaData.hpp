#pragma once

#include <Enlivengine/System/Hash.hpp>

#include <array>
#include <initializer_list>

namespace en
{

// https://stackoverflow.com/questions/81870/is-it-possible-to-print-a-variables-type-in-standard-c/56766138#56766138
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

template <typename T, std::size_t N>
class ConstexprVector
{
public:
	using storage_t = std::array<T, N>;
	using iterator = typename storage_t::iterator;
	using const_iterator = typename storage_t::const_iterator;

	constexpr ConstexprVector() : mData(), mSize(0) {}
	constexpr ConstexprVector(std::initializer_list<T> list) : mData(list), mSize(list.size()) {}

	constexpr auto begin() const { return mData.begin(); }
	constexpr auto end() const { return mData.begin() + mSize; }

	constexpr const T& operator[](std::size_t index) const { return mData[index]; }

	constexpr std::size_t size() const { return mSize; }
	constexpr std::size_t capacity() const { return N; }

	constexpr void push_back(T value)
	{
		mData[mSize++] = std::move(value);
	}

	constexpr void pop_back()
	{
		mSize--;
	}

private:
	storage_t mData;
	std::size_t mSize;
};

class MetaDataAttribute
{
public:
	constexpr MetaDataAttribute() : mValue(), mName() {}
	constexpr MetaDataAttribute(std::string_view value) : mValue(value), mName(value) {} // TODO : Parse Name/SubValue...

	constexpr std::string_view Get() const { return mValue; }
	constexpr std::string_view GetName() const { return mName; }

private:
	std::string_view mValue;
	std::string_view mName;
};

using MetaDataAttributeArray = ConstexprVector<MetaDataAttribute, 8>; // GENERATED

class MetaDataEnumValue
{
public:
	constexpr MetaDataEnumValue() = delete;
	constexpr MetaDataEnumValue(std::string_view name, U32 id, U32 value, MetaDataAttributeArray attributes) : mName(name), mID(id), mValue(value), mAttributes(attributes) {}

	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetID() const { return mID; }
	constexpr U32 GetValue() const { return mValue; }
	// TODO : Attributes accessors

	constexpr bool operator==(const MetaDataEnumValue& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataEnumValue& other) const { return mID != other.mID; }

private:
	std::string_view mName;
	U32 mID;
	U32 mValue;
	MetaDataAttributeArray mAttributes;
};

using MetaDataEnumValueArray = ConstexprVector<MetaDataEnumValue, 8>; // GENERATED

class MetaDataEnum
{
public:
	constexpr MetaDataEnum() = delete;
	constexpr MetaDataEnum(std::string_view name, U32 id, MetaDataEnumValueArray values, MetaDataAttributeArray attributes) : mName(name), mID(id), mValues(values), mAttributes(attributes) {}

	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetID() const { return mID; }
	// TODO : Values accessors
	// TODO : Attributes accessors

	constexpr bool operator==(const MetaDataEnum& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataEnum& other) const { return mID != other.mID; }

private:
	std::string_view mName;
	U32 mID;
	MetaDataEnumValueArray mValues;
	MetaDataAttributeArray mAttributes;
};

class MetaDataType
{
public:
	constexpr MetaDataType() = delete;
	constexpr MetaDataType(std::string_view name, U32 id, U32 size, MetaDataAttributeArray attributes) : mName(name), mID(id), mSize(size), mAttributes(attributes) {}

	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetID() const { return mID; }
	constexpr U32 GetSize() const { return mSize; }
	// TODO : Attributes accessors

	constexpr bool operator==(const MetaDataType& other) const { return mID == other.mID; }
	constexpr bool operator!=(const MetaDataType& other) const { return mID != other.mID; }

private:
    std::string_view mName;
	U32 mID;
    U32 mSize;
	MetaDataAttributeArray mAttributes;
};

class MetaDataProperty
{
public:
	constexpr MetaDataProperty(const MetaDataType& type, std::string_view name, U32 id, MetaDataAttributeArray attributes) : mType(type), mName(name), mID(id), mAttributes(attributes) {}

	constexpr const MetaDataType& GetType() const { return mType; }
	constexpr std::string_view GetName() const { return mName; }
	constexpr U32 GetID() const { return mID; }

private:
	const MetaDataType& mType;
	std::string_view mName;
	U32 mID;
	MetaDataAttributeArray mAttributes;
};

class MetaDataTypes
{
public:
	MetaDataTypes() = delete;

	static constexpr const MetaDataType& GetType(std::string_view typeName)
	{
		return GetType(Hash::CRC32(typeName));
	}

	template <typename T>
	static constexpr const MetaDataType& GetType()
	{
		return GetType(GetTypeHash<T>());
	}

	static constexpr const MetaDataType& GetType(U32 id)
	{
		for (auto& type : kTypes)
		{
			if (type.GetID() == id)
			{
				return type;
			}
		}
		return kInvalidType;
	}

	static constexpr U32 GetTypeCount() { return kTypeCount; }

	static constexpr const MetaDataType& GetTypeByIndex(U32 index) { return kTypes[index]; }

private:
	static constexpr MetaDataType kInvalidType = MetaDataType("InvalidType", 0, 0, {});
	static constexpr U32 kTypeCount = 4; // GENERATED
	static constexpr std::array<MetaDataType, kTypeCount> kTypes = {
		MetaDataType("int", Hash::CRC32("int"), 4, {}), // GENERATED
		MetaDataType("float", Hash::CRC32("float"), 4, {}), // GENERATED
		MetaDataType("en::TestClassA", Hash::CRC32("en::TestClassA"), 8, {}), // GENERATED
		MetaDataType("en::TestClassB", Hash::CRC32("en::TestClassB"), 8, {}) // GENERATED
	};
};

} // namespace en