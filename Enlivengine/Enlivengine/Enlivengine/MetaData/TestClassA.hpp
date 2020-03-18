#pragma once

#include <Enlivengine/MetaData/MetaDataBase.hpp>

namespace en
{

ENLIVE_META_ENUM(MyEnum)
{
    A,
    B,
    C
};
ENLIVE_META_ENUM_DEF(MyEnum)
	ENLIVE_META_ENUM_VALUE(MyEnum, A) ENLIVE_METADATA_COMMA()
	ENLIVE_META_ENUM_VALUE(MyEnum, B) ENLIVE_METADATA_COMMA()
	ENLIVE_META_ENUM_VALUE(MyEnum, C)
ENLIVE_META_ENUM_DEF_END(MyEnum)

class TestClassA
{
public:
	ENLIVE_META_CLASS_DECL(TestClassA)

	U32 GetA() const { return a; }
	I32 GetB() const { return b; }
	F32 GetC() const { return c; }
    MyEnum GetD() const { return d; }
    bool GetE() const { return e; }

	void SetA(U32 v) { a = v; }
	void SetB(I32 v) { b = v; }
	void SetC(F32 v) { c = v; }
    void SetD(MyEnum v) { d = v; }
    void SetE(bool v) { e = v; }

protected:
	U32 a;
	I32 b;
	F32 c;
    MyEnum d;
    bool e;
};

ENLIVE_META_CLASS_DEF(TestClassA)
	ENLIVE_META_CLASS_PROPERTY(en::TestClassA, a, en::PrimitivesMetaData::GetType<en::U32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassA, b, en::PrimitivesMetaData::GetType<en::I32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassA, c, en::PrimitivesMetaData::GetType<en::F32>()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassA, d, en::MetaData_MyEnum::GetMetaData()) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestClassA, e, en::PrimitivesMetaData::GetType<bool>())
ENLIVE_META_CLASS_DEF_END(TestClassA, en::TestClassA)

} // namespace en