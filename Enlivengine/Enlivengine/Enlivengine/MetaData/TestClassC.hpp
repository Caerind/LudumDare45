#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

template <typename T>
class TestArrayTemplate
{
public:
	template <typename T> friend class MetaData_TestArrayTemplate;
	static constexpr const en::MetaDataType& GetStaticMetaData();

	const T& GetElement() const { return element; }

	void SetElement(const T& v) { element = v; }

private:
	T element;
};

template <typename T>
class MetaData_TestArrayTemplate
{
public:
	constexpr MetaData_TestArrayTemplate() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr const en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestArrayTemplate<T>::element"), en::PrimitivesMetaData::GetType<T>(), "element", ENLIVE_OFFSET_OF(en::TestArrayTemplate<T>, element), ENLIVE_SIZE_OF_MEMBER(en::TestArrayTemplate<T>, element), en::PrimitivesMetaData::GetType<T>().GetTraits())
	};
	static constexpr const en::MetaDataType* s_MetaDataTemplateTypes[] =
	{
		&en::PrimitivesMetaData::GetType<T>()
	};
	static constexpr const en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestArrayTemplate<T>"), "TestArrayTemplate<T>", ENLIVE_SIZE_OF(en::TestArrayTemplate<T>), ENLIVE_ALIGN_OF(en::TestArrayTemplate<T>), en::TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties), en::Attribute_None, nullptr, s_MetaDataTemplateTypes, 0);
};
template <typename T>
constexpr const en::MetaDataType& TestArrayTemplate<T>::GetStaticMetaData()
{
	return en::MetaData_TestArrayTemplate<T>::GetMetaData();
}


class TestClassC : public TestClassA
{
public:
	friend class MetaData_TestClassC;
	static constexpr const en::MetaDataType& GetStaticMetaData();

	TestArrayTemplate<en::U32> GetB() const { return b; }
	TestArrayTemplate<en::F32> GetC() const { return c; }

	void SetB(TestArrayTemplate<en::U32> v) { b = v; }
	void SetC(TestArrayTemplate<en::F32> v) { c = v; }

private:
    TestArrayTemplate<en::U32> b;
	TestArrayTemplate<en::F32> c;
};

class MetaData_TestClassC
{
public:
	constexpr MetaData_TestClassC() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr const en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestClassC::b"), en::MetaData_TestArrayTemplate<en::U32>::GetMetaData(), "b", ENLIVE_OFFSET_OF(en::TestClassC, b), ENLIVE_SIZE_OF_MEMBER(en::TestClassC, b), en::MetaData_TestArrayTemplate<en::U32>::GetMetaData().GetTraits()),
		en::MetaDataProperty(en::Hash::CRC32("TestClassC::c"), en::MetaData_TestArrayTemplate<en::F32>::GetMetaData(), "c", ENLIVE_OFFSET_OF(en::TestClassC, c), ENLIVE_SIZE_OF_MEMBER(en::TestClassC, c), en::MetaData_TestArrayTemplate<en::F32>::GetMetaData().GetTraits())
	};
	static constexpr const en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestClassC"), "TestClassC", ENLIVE_SIZE_OF(en::TestClassC), ENLIVE_ALIGN_OF(en::TestClassC), TypeTraits_Class, &en::MetaData_TestClassA::GetMetaData(), s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties));
};
constexpr const en::MetaDataType& TestClassC::GetStaticMetaData()
{
	return en::MetaData_TestClassC::GetMetaData();
}

} // namespace en