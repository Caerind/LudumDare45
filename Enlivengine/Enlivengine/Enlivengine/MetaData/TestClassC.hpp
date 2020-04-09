#pragma once

#include <Enlivengine/MetaData/MetaData.hpp>

#include <Enlivengine/MetaData/TestClassA.hpp>
#include <Enlivengine/System/Time.hpp>

class TestClassC : public en::TestClassA
{
public:
	ENLIVE_META_CLASS_DECL(TestClassC)

	en::TestArray<en::U32>& GetB() { return b; }
	const en::TestArray<en::U32>& GetB() const { return b; }
	en::TestArray<en::TestClassA>& GetC() { return c; }
	const en::TestArray<en::TestClassA>& GetC() const { return c; }

	void SetDuration(const en::Time& t) { d = t; }
	const en::Time& GetDuration() const { return d; }

private:
	en::TestArray<en::U32> b;
	en::TestArray<en::TestClassA> c;
	en::Time d;
};

ENLIVE_META_CLASS_DEF(TestClassC)
	ENLIVE_META_CLASS_PROPERTY(en::U32, TestClassA::a) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::I32, TestClassA::b) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::F32, TestClassA::c) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::MyEnum, TestClassA::d) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(bool, TestClassA::e) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestArray<en::U32>, b) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::TestArray<en::TestClassA>, c) ENLIVE_METADATA_COMMA()
	ENLIVE_META_CLASS_PROPERTY(en::Time, d)
ENLIVE_META_CLASS_DEF_END_EX(&en::MetaData<en::TestClassA>::Get())

ENLIVE_META_CLASS_IMPL(TestClassC)