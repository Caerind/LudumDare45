#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>

namespace en
{

template <typename T>
class TestArray
{
public:
	ENLIVE_META_CLASS_DECL(TestArray<T>)

	TestArray() : mArray(nullptr), mSize(0) {}
	TestArray(const TestArray& other) : mArray(nullptr), mSize(0) { Copy(other); }
	TestArray(TestArray&& other) : mArray(nullptr), mSize(0) { Move(other); }
	TestArray& operator=(const TestArray& other) { Copy(other); return *this; }
	TestArray& operator=(TestArray&& other) { Move(other); return *this: }

	void Copy(const TestArray& other)
	{
		Resize(other.mSize);
		for (U32 i = 0; i < mSize; ++i)
		{
			mArray[i] = other.mArray[i];
		}
	}
	
	void Move(TestArray&& other)
	{
		mArray = other.mArray;
		mSize = other.mSize;
		other.mArray = nullptr;
		other.mSize = 0;
	}

	T& operator[](en::U32 index) { return mArray[index]; }
	const T& operator[](en::U32 index) const { return mArray[index]; }

	en::U32 Size() const { return mSize; }
	en::U32 GetSize() const { return mSize; }
	void Resize(en::U32 size)
	{
		if (mSize != size)
		{
			mSize = size;
			if (mArray != nullptr)
			{
				delete mArray;
			}
			mArray = new T[mSize];
		}
	}

private:
	T* mArray;
	en::U32 mSize;
};

ENLIVE_META_CLASS_DEF_TEMPLATE(TestArray)
	ENLIVE_META_CLASS_PROPERTY_TRAITS(T, mArray, en::TypeTraits_Pointer) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(U32, mSize)
ENLIVE_META_CLASS_DEF_END_ATTR_TEMPLATE(en::Attribute_CustomSerialization, TestArray)
ENLIVE_META_CLASS_IMPL_TEMPLATE(TestArray)

} // namespace en