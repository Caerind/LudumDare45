#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

template <typename T>
class Array
{
public:
	ENLIVE_META_CLASS_DECL(Array<T>);

	constexpr Array() = default;
	constexpr Array(const Array& other) = delete;
	constexpr Array(Array&& other) { Move(other); }

	constexpr Array& operator=(const Array& other) = delete;
	constexpr Array& operator=(Array& other) { Move(other); return *this; }

	constexpr bool operator==(const Array& other) const { return Equals(other); }
	constexpr bool operator!=(const Array& other) const { return !Equals(other); }
	constexpr bool Equals(const Array& other) const { return mSize != other.mSize && std::memcmp((const void*)mArray, (const void*)other.mArray, mSize * ENLIVE_SIZE_OF(T)) == 0; }

	constexpr void Copy(const Array& other)
	{
		Resize(other.mSize);
		std::memcpy((void*)mArray, (const void*)other.mArray, other.mSize * ENLIVE_SIZE_OF(T));
	}

	constexpr void Move(Array&& other)
	{
		mArray = other.mArray;
		mSize = other.mSize;
		mCapacity = other.mCapacity;
		other.mArray = nullptr;
		other.mSize = 0;
		other.mCapacity = 0;
	}

	void Resize(U32 newSize)
	{
		if (newSize > mCapacity)
		{
			Reserve(newSize);
		}
		else if (newSize == 0)
		{
			Clear();
		}
		mSize = newSize;
	}

	void Reserve(U32 newCapacity)
	{
		if (newCapacity > mCapacity)
		{
			const T* oldArray = mArray;
			mCapacity = newCapacity;
			mArray = new T[mCapacity];
			if (oldArray != nullptr)
			{
				std::memcpy((void*)mArray, (const void*)oldArray, mSize * ENLIVE_SIZE_OF(T));
				delete[] oldArray;
			}
		}
	}

	constexpr void ResetSize() { mSize = 0; }

	void Clear()
	{
		mSize = 0;
		if (mArray != nullptr)
		{
			delete[] mArray;
			mArray = nullptr;
		}
		mCapacity = 0;
	}

	constexpr void Sort() { std::sort(begin(), end()); }
	template <typename Predicate> constexpr void Sort(Predicate predicate) { std::sort(begin(), end(), predicate); }

	constexpr T& operator[](U32 index) { assert(index < mSize); return mArray[index]; }
	constexpr const T& operator[](U32 index) const { assert(index < mSize); return mArray[index]; }

	constexpr U32 Size() const { return mSize; }
	constexpr U32 GetSize() const { return mSize; }
	constexpr bool Empty() const { return mSize == 0; }

	constexpr U32 GetElementSize() const { return ENLIVE_SIZE_OF(T); }
	constexpr U32 GetElementAlignment() const { return ENLIVE_ALIGN_OF(T); }

	constexpr U32 Capacity() const { return mCapacity; }
	constexpr U32 GetCapacity() const { return mCapacity; }

	constexpr U8* GetArrayData() { return (U8*)mArray; }
	constexpr const U8* GetArrayData() const { return (U8*)mArray; }
	constexpr U32 GetArrayDataSize() const { return mSize * ENLIVE_SIZE_OF(T); }

	constexpr T& Front() { assert(mSize > 0); return mArray[0]; }
	constexpr const T& Front() const { assert(mSize > 0); return mArray[0]; }
	constexpr T& Back() { assert(mSize > 0); return mArray[mSize - 1]; }
	constexpr const T& Back() const { assert(mSize > 0); return mArray[mSize - 1]; }

private:
	T* mArray = nullptr;
	U32 mSize = 0;
	U32 mCapacity = 0;
};

ENLIVE_META_CLASS_DEF_TEMPLATE(Array)
	ENLIVE_META_CLASS_PROPERTY_TRAITS(T, mArray, en::TypeTraits_Pointer) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(U32, mSize) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(U32, mCapacity)
ENLIVE_META_CLASS_DEF_END_ATTR_TEMPLATE(en::Attribute_CustomSerialization, Array)
ENLIVE_META_CLASS_IMPL_TEMPLATE(Array)

} // namespace en