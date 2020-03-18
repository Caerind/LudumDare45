#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>
#include <Enlivengine/MetaData/TestClassA.hpp>

namespace en
{

class TestClassB
{
public:
	ENLIVE_META_CLASS_DECL(TestClassB)

	TestClassA* GetA() const { return a; }
	F32 GetB() const { return b; }
    U32 GetC() const { return c; }
    F32* GetD() const { return d; }
    MyEnum* GetE() const { return e; }
    const TestClassA& GetF(U32 index) const { return f[index]; }
    F32* GetG(U32 index) const { return g[index]; }

	void SetA(TestClassA* v) { a = v; }
	void SetB(F32 v) { b = v; }
	void SetC(U32 v) { c = v; }
	void SetD(F32* v) { d = v; }
	void SetE(MyEnum* v) { e = v; }
    void SetF(U32 index, const TestClassA& v) { f[index] = v; }
    void SetG(U32 index, F32* v) { g[index] = v; }

private:
    TestClassA* a; // Class/Ptr
    F32 b; // Primitive
    U32 c; // Primitive
    F32* d; // Primitive/Ptr
    MyEnum* e; // Enum/Ptr
	TestClassA f[3]; // Class/Array
	F32* g[2]; // Primitive/Array/Ptr
	// TODO : U32 (*h)[]; // Primitive/ArrayPtr
	// TODO : I32* (*i)[]; // Primitive/Ptr/ArrayPtr
};

ENLIVE_META_CLASS_DEF(TestClassB)
	ENLIVE_META_CLASS_PROPERTY_EX(en::TestClassB, a, en::MetaData_TestClassA::GetMetaData(), en::TypeTraits_Pointer) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassB, b, en::PrimitivesMetaData::GetType<en::F32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassB, c, en::PrimitivesMetaData::GetType<en::U32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY_EX(en::TestClassB, d, en::PrimitivesMetaData::GetType<en::F32>(), en::TypeTraits_Pointer) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY_EX(en::TestClassB, e, en::MetaData_MyEnum::GetMetaData(), en::TypeTraits_Pointer) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY_ARRAY(en::TestClassB, f, en::MetaData_TestClassA::GetMetaData(), 3) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY_EX_ARRAY(en::TestClassB, g, en::PrimitivesMetaData::GetType<en::F32>(), en::TypeTraits_Pointer, 2)
	// TODO : en::MetaDataProperty(en::Hash::CRC32("TestClassB::h"), en::PrimitivesMetaData::GetType<en::U32>(), "h", ENLIVE_OFFSET_OF(en::TestClassB, h), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, h), en::PrimitivesMetaData::GetType<en::U32>().GetTraits() | en::TypeTraits_ArrayPtr),
	// TODO : en::MetaDataProperty(en::Hash::CRC32("TestClassB::i"), en::PrimitivesMetaData::GetType<en::I32>(), "i", ENLIVE_OFFSET_OF(en::TestClassB, i), ENLIVE_SIZE_OF_MEMBER(en::TestClassB, i), en::PrimitivesMetaData::GetType<en::I32>().GetTraits() | en::TypeTraits_ArrayPtr | en::TypeTraits_Pointer)
ENLIVE_META_CLASS_DEF_END(TestClassB, en::TestClassB)

} // namespace en