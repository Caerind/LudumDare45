#include <Enlivengine/Graphics/AnimationStateMachine.hpp>

namespace en
{

/*

AnimationStateMachine::Parameter::Parameter(const std::string& name, ParameterType type)
	: mName(name)
	, mHashedName(Hash::CRC32(name.c_str()))
{
	SetType(type);
}

void AnimationStateMachine::Parameter::SetType(ParameterType type)
{
	if (mType != type)
	{
		mType = type;

		switch (type)
		{
		case ParameterType::Boolean: SetBooleanValue(false); break;
		case ParameterType::Float: SetFloatValue(0.0f); break;
		case ParameterType::Integer: SetIntegerValue(0); break;
		case ParameterType::Trigger: SetTriggerValue(false); break;
		default: assert(false); break;
		}
	}
}

void AnimationStateMachine::Parameter::SetBooleanValue(bool value)
{
	assert(mType == ParameterType::Boolean);
	mValue.bValue = value;
}

void AnimationStateMachine::Parameter::SetFloatValue(F32 value)
{
	assert(mType == ParameterType::Float);
	mValue.fValue = value;
}

void AnimationStateMachine::Parameter::SetIntegerValue(I32 value)
{
	assert(mType == ParameterType::Integer);
	mValue.iValue = value;
}

void AnimationStateMachine::Parameter::SetTriggerValue(bool value)
{
	assert(mType == ParameterType::Trigger);
	mValue.bValue = value;
}

bool AnimationStateMachine::Parameter::GetBooleanValue() const
{
	assert(mType == ParameterType::Boolean);
	return mValue.bValue;
}

F32 AnimationStateMachine::Parameter::GetFloatValue() const
{
	assert(mType == ParameterType::Float);
	return mValue.fValue;
}

I32 AnimationStateMachine::Parameter::GetIntegerValue() const
{
	assert(mType == ParameterType::Integer);
	return mValue.iValue;
}

bool AnimationStateMachine::Parameter::GetTriggerValue() const
{
	assert(mType == ParameterType::Trigger);
	return mValue.bValue;
}

AnimationStateMachine::Condition::Condition(Operator op)
	: mOperator(Operator::Equal)
{
	SetBoolean(true);
}

AnimationStateMachine::Condition::Condition(bool value)
	: mOperator(Operator::Equal)
{
	SetBoolean(value);
}

AnimationStateMachine::Condition::Condition(Operator op, F32 value)
	: mOperator(op)
{
	SetFloat(value);
}

AnimationStateMachine::Condition::Condition(Operator op, I32 value)
	: mOperator(op)
{
	SetInteger(value);
}

bool AnimationStateMachine::Condition::GetBoolean() const
{
	assert(mType == ParameterType::Boolean);
	return mValue.bValue;
}

F32 AnimationStateMachine::Condition::GetFloat() const
{
	assert(mType == ParameterType::Float);
	return mValue.fValue;
}

I32 AnimationStateMachine::Condition::GetInteger() const
{
	assert(mType == ParameterType::Integer);
	return mValue.iValue;
}

void AnimationStateMachine::Condition::SetBoolean(bool value)
{
	mType = ParameterType::Boolean;
	mValue.bValue = value;
}

void AnimationStateMachine::Condition::SetFloat(F32 value)
{
	mType = ParameterType::Float;
	mValue.fValue = value;
}

void AnimationStateMachine::Condition::SetInteger(I32 value)
{
	mType = ParameterType::Integer;
	mValue.iValue = value;
}

bool AnimationStateMachine::Condition::Evaluate(const Parameter& parameter) const
{
	assert(parameter.GetType() == GetType());
	switch (parameter.GetType())
	{
	case ParameterType::Boolean: return Evaluate(parameter.GetBoolean());
	case ParameterType::Float: return Evaluate(parameter.GetFloat());
	case ParameterType::Integer: return Evaluate(parameter.GetInteger());
	default: assert(false);
	}
	return false;
}

bool AnimationStateMachine::Condition::Evaluate(bool value) const
{
	switch (mOperator)
	{
	case Operator::Equal:
	case Operator::LessEq:
	case Operator::GreaterEq:
		return value == mValue.bValue;
	case Operator::NotEqual:
	case Operator::Less:
	case Operator::Greater:
		return value != mValue.bValue;
	default:
		assert(false);
	}
	return false;
}

bool AnimationStateMachine::Condition::Evaluate(F32 value) const
{
	switch (mOperator)
	{
	case Operator::Equal: return value == mValue.fValue;
	case Operator::NotEqual: return value != mValue.fValue;
	case Operator::Less: return value < mValue.fValue;
	case Operator::LessEq: return value <= mValue.fValue;
	case Operator::Greater: return value > mValue.fValue;
	case Operator::GreaterEq: return value >= mValue.fValue;
	default: assert(false);
	}
	return false;
}

bool AnimationStateMachine::Condition::Evaluate(I32 value) const
{
	switch (mOperator)
	{
	case Operator::Equal: return value == mValue.iValue;
	case Operator::NotEqual: return value != mValue.iValue;
	case Operator::Less: return value < mValue.iValue;
	case Operator::LessEq: return value <= mValue.iValue;
	case Operator::Greater: return value > mValue.iValue;
	case Operator::GreaterEq: return value >= mValue.iValue;
	default: assert(false);
	}
	return false;
}

bool AnimationStateMachine::StateTransition::CanTransit() const
{
	for (const Condition& condition : mConditions)
	{
		if (!condition.Evaluate())
		{

		}
	}
	return true;
}

*/

} // namespace en