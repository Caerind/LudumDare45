#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

template <typename T>
class TestArrayTemplate
{
public:
	//template <typename T> friend class MetaData_TestArrayTemplate;
	//static constexpr const en::MetaDataType& GetStaticMetaData();

	const T& GetElement() const { return element; }

	void SetElement(const T& v) { element = v; }

private:
	T element;
};

/*
template <typename T>
class MetaData_TestArrayTemplate
{
public:
	constexpr MetaData_TestArrayTemplate() = delete;
	static constexpr const en::MetaDataType& GetMetaData() { return s_MetaData; }
private:
	static constexpr const en::MetaDataProperty s_MetaDataProperties[] =
	{
		en::MetaDataProperty(en::Hash::CRC32("TestArrayTemplate<T>::element"), en::PrimitivesMetaData::GetType<T>(), "element", ENLIVE_OFFSET_OF(TestArrayTemplate<T>, element), ENLIVE_SIZE_OF_MEMBER(TestArrayTemplate<T>, element), en::PrimitivesMetaData::GetType<T>().GetTraits())
	};
	static constexpr const en::MetaDataType* s_MetaDataTemplateTypes[] =
	{
		&en::PrimitivesMetaData::GetType<T>()
	};
	static constexpr const en::MetaDataType s_MetaData = en::MetaDataType(en::Hash::CRC32("TestArrayTemplate<T>"), "TestArrayTemplate<T>", ENLIVE_SIZE_OF(TestArrayTemplate<T>), ENLIVE_ALIGN_OF(TestArrayTemplate<T>), en::TypeTraits_Class, nullptr, s_MetaDataProperties, ENLIVE_ARRAY_SIZE(s_MetaDataProperties), en::Attribute_None, nullptr, s_MetaDataTemplateTypes, 0);
};
template <typename T>
constexpr const en::MetaDataType& TestArrayTemplate<T>::GetStaticMetaData()
{
	return en::MetaData_TestArrayTemplate<T>::GetMetaData();
}
*/


class TestClassC : public en::TestClassA
{
public:
	ENLIVE_META_CLASS_DECL(TestClassC)

	TestArrayTemplate<en::U32> GetB() const { return b; }
	TestArrayTemplate<en::F32> GetC() const { return c; }

	void SetB(TestArrayTemplate<en::U32> v) { b = v; }
	void SetC(TestArrayTemplate<en::F32> v) { c = v; }

private:
    TestArrayTemplate<en::U32> b;
	TestArrayTemplate<en::F32> c;
};

ENLIVE_META_CLASS_DEF(TestClassC)
	ENLIVE_META_CLASS_PROPERTY(TestClassC, TestClassA::a, en::PrimitivesMetaData::GetType<en::U32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, TestClassA::b, en::PrimitivesMetaData::GetType<en::I32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, TestClassA::c, en::PrimitivesMetaData::GetType<en::F32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, TestClassA::d, en::MetaData_MyEnum::GetMetaData()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, TestClassA::e, en::PrimitivesMetaData::GetType<bool>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, b, MetaData_TestArrayTemplate<en::U32>::GetMetaData()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(TestClassC, c, MetaData_TestArrayTemplate<en::F32>::GetMetaData())
ENLIVE_META_CLASS_DEF_END_EX(TestClassC, TestClassC, &en::MetaData_TestClassA::GetMetaData())