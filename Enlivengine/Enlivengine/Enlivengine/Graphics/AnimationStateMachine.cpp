#include <Enlivengine/Graphics/AnimationStateMachine.hpp>

namespace en
{

AnimationStateMachine::State::State(const std::string& name, U32 clipIndex)
{
    SetName(name);
    SetClipIndex(clipIndex);
    mSpeedScale = 1.0f;
    mExitOnlyAtEnd = false;
}

void AnimationStateMachine::State::SetName(const std::string& name)
{
    const U32 hashedName = Hash::CRC32(name.c_str());
    if (mHashedName != hashedName)
    {
        mName = name;
        mHashedName = hashedName;
    }
}

AnimationStateMachine::Parameter::Parameter(const std::string& name, Parameter::Type type)
{
    SetName(name);
	SetType(type);
}

void AnimationStateMachine::Parameter::SetName(const std::string& name)
{
    const U32 hashedName = Hash::CRC32(name.c_str());
    if (mHashedName != hashedName)
    {
        mName = name;
        mHashedName = hashedName;
    }
}

void AnimationStateMachine::Parameter::SetType(Parameter::Type type)
{
	if (mType != type)
	{
		mType = type;

		switch (type)
		{
        case Parameter::Type::Boolean: SetBooleanValue(false); break;
        case Parameter::Type::Float: SetFloatValue(0.0f); break;
        case Parameter::Type::Integer: SetIntegerValue(0); break;
        case Parameter::Type::Trigger: SetTriggerValue(false); break;
		default: assert(false); break;
		}
	}
}

void AnimationStateMachine::Parameter::SetBooleanValue(bool value)
{
	assert(mType == Parameter::Type::Boolean);
	mValue.bValue = value;
}

void AnimationStateMachine::Parameter::SetFloatValue(F32 value)
{
	assert(mType == Parameter::Type::Float);
	mValue.fValue = value;
}

void AnimationStateMachine::Parameter::SetIntegerValue(I32 value)
{
	assert(mType == Parameter::Type::Integer);
	mValue.iValue = value;
}

void AnimationStateMachine::Parameter::SetTriggerValue(bool value)
{
	assert(mType == Parameter::Type::Trigger);
	mValue.bValue = value;
}

bool AnimationStateMachine::Parameter::GetBooleanValue() const
{
	assert(mType == Parameter::Type::Boolean);
	return mValue.bValue;
}

F32 AnimationStateMachine::Parameter::GetFloatValue() const
{
	assert(mType == Parameter::Type::Float);
	return mValue.fValue;
}

I32 AnimationStateMachine::Parameter::GetIntegerValue() const
{
	assert(mType == Parameter::Type::Integer);
	return mValue.iValue;
}

bool AnimationStateMachine::Parameter::GetTriggerValue() const
{
	assert(mType == Parameter::Type::Trigger);
	return mValue.bValue;
}

AnimationStateMachine::Condition::Condition()
{
    // TODO : No idea what to put here...
    mParameterIndex = U32_Max;
    mOperator = Operator::Equal;
    mOperand.bValue = false;
}

AnimationStateMachine::Transition::Transition(U32 fromState, U32 toState)
{
    // TODO : TODO
}

void AnimationStateMachine::Transition::AddCondition(U32 conditionIndex)
{
    // TODO : TODO
}

void AnimationStateMachine::Transition::RemoveCondition(U32 conditionIndex)
{
    // TODO : TODO
}

void AnimationStateMachine::Transition::ClearConditions()
{
    // TODO : TODO
}

bool AnimationStateMachine::LoadFromFile(const std::string& filename)
{
    // TODO : TODO
    return true;
}

bool AnimationStateMachine::SaveToFile(const std::string& filename)
{
    // TODO : TODO
    return true;
}

void AnimationStateMachine::Clean()
{
    // TODO : TODO
}

void AnimationStateMachine::SetAnimation(AnimationPtr animation)
{
    mAnimation = animation;
}

const AnimationPtr& AnimationStateMachine::GetAnimation() const
{
    return mAnimation;
}

U32 AnimationStateMachine::AddState(const std::string& name, U32 clipIndex)
{
    // TODO : TODO
    return 0;
}

void AnimationStateMachine::RemoveState(U32 index)
{
    // TODO : TODO
}

void AnimationStateMachine::ClearStates()
{
    // TODO : TODO
}

void AnimationStateMachine::SetStateName(U32 index, const std::string& name)
{
    // TODO : TODO
}

void AnimationStateMachine::SetStateClipIndex(U32 index, U32 clipIndex)
{
    // TODO : TODO
}

void AnimationStateMachine::SetStateSpeedScale(U32 index, F32 speedScale)
{
    // TODO : TODO
}

void AnimationStateMachine::SetStateExitOnlyAtEnd(U32 index, bool exitOnlyAtEnd)
{
    // TODO : TODO
}

U32 AnimationStateMachine::GetStateCount() const
{
    // TODO : TODO
    return 0;
}

const AnimationStateMachine::State& AnimationStateMachine::GetState(U32 index) const
{
    // TODO : TODO
    return mStates[0];
}

U32 AnimationStateMachine::GetStateIndexByName(const std::string& name) const
{
    // TODO : TODO
    return 0;
}

U32 AnimationStateMachine::GetStateIndexByName(U32 hashedName) const
{
    // TODO : TODO
    return 0;
}

U32 AnimationStateMachine::AddParameter(const std::string& name, Parameter::Type type)
{
    // TODO : TODO
    return 0;
}

void AnimationStateMachine::RemoveParameter(U32 index)
{
    // TODO : TODO
}

void AnimationStateMachine::ClearParameters()
{
    // TODO : TODO
}

void AnimationStateMachine::SetParameterName(U32 index, const std::string& name)
{
    // TODO : TODO
}

void AnimationStateMachine::SetParameterType(U32 index, Parameter::Type type)
{
    // TODO : TODO
}

void AnimationStateMachine::SetParameterBoolean(U32 index, bool value)
{
    // TODO : TODO
}

void AnimationStateMachine::SetParameterFloat(U32 index, F32 value)
{
    // TODO : TODO
}

void AnimationStateMachine::SetParameterInteger(U32 index, I32 value)
{
    // TODO : TODO
}

U32 AnimationStateMachine::GetParameterCount() const
{
    // TODO : TODO
    return 0;
}

const AnimationStateMachine::Parameter& AnimationStateMachine::GetParameter(U32 index) const
{
    // TODO : TODO
    return mParameters[0];
}

U32 AnimationStateMachine::GetParameterIndexByName(const std::string& name) const
{
    // TODO : TODO
    return 0;
}

U32 AnimationStateMachine::GetParameterIndexByName(U32 hashedName) const
{
    // TODO : TODO
    return 0;
}

U32 AnimationStateMachine::AddCondition(U32 parameterIndex)
{
    assert(parameterIndex < GetParameterCount());
    Condition cond;
    cond.SetParameterIndex(parameterIndex);
    mConditions.emplace_back(cond);
    return GetConditionCount() - 1;
}

void AnimationStateMachine::RemoveCondition(U32 index)
{
    assert(index < GetConditionCount());
    // TODO : TODO
}

void AnimationStateMachine::ClearConditions()
{
    // TODO : TODO
}

void AnimationStateMachine::SetConditionParameter(U32 index, U32 parameterIndex)
{
    assert(index < GetConditionCount());
    assert(parameterIndex < GetParameterCount());

    mConditions[index].SetParameterIndex(parameterIndex);

    switch (type)
    {
    case Parameter::Type::Boolean: mConditions[index].SetBooleanValue(false); break;
    case Parameter::Type::Float: mConditions[index].SetFloatValue(0.0f); break;
    case Parameter::Type::Integer: mConditions[index].SetIntegerValue(0); break;
    case Parameter::Type::Trigger: mConditions[index].SetTriggerValue(false); break;
    default: assert(false); break;
    }
}

void AnimationStateMachine::SetConditionOperator(U32 index, Condition::Operator operat)
{
    assert(index < GetConditionCount());

    // TODO : Error, assert or no effect if not matching the parameter type properly ? (Trigger/Boolean)
#ifdef ENLIVE_ENABLE_ASSERT
    const U32 parameterIndex = mConditions[index].GetParameterIndex();
    assert(parameterIndex < GetParameterCount());
    const Parameter::Type parameterType = mParameters[parameterIndex].GetType();
    assert(parameterType != Parameter::Type::Trigger);
    if (parameterType == Parameter::Type::Boolean)
    {
        assert(operat == Condition::Operator::Equal || Condition::Operator::NotEqual);
    }
#endif // ENLIVE_ENABLE_ASSERT

    mConditions[index].SetOperator(operat);
}

void AnimationStateMachine::SetConditionOperandBoolean(U32 index, bool operand)
{
#ifdef ENLIVE_ENABLE_ASSERT
    assert(index < GetConditionCount());
    const U32 parameterIndex = mConditions[index].GetParameterIndex();
    assert(parameterIndex < GetParameterCount());
    const Parameter::Type parameterType = mParameters[parameterIndex].GetType();
    assert(parameterType == Parameter::Type::Boolean);
#endif // ENLIVE_ENABLE_ASSERT

    mConditions[index].SetOperandBoolean(operand);
}

void AnimationStateMachine::SetConditionOperandFloat(U32 index, F32 operand)
{
#ifdef ENLIVE_ENABLE_ASSERT
    assert(index < GetConditionCount());
    const U32 parameterIndex = mConditions[index].GetParameterIndex();
    assert(parameterIndex < GetParameterCount());
    const Parameter::Type parameterType = mParameters[parameterIndex].GetType();
    assert(parameterType == Parameter::Type::Float);
#endif // ENLIVE_ENABLE_ASSERT

    mConditions[index].SetOperandFloat(operand);
}

void AnimationStateMachine::SetConditionOperandInteger(U32 index, I32 operand)
{
#ifdef ENLIVE_ENABLE_ASSERT
    assert(index < GetConditionCount());
    const U32 parameterIndex = mConditions[index].GetParameterIndex();
    assert(parameterIndex < GetParameterCount());
    const Parameter::Type parameterType = mParameters[parameterIndex].GetType();
    assert(parameterType == Parameter::Type::Integer);
#endif // ENLIVE_ENABLE_ASSERT

    mConditions[index].SetOperandInteger(operand);
}

U32 AnimationStateMachine::GetConditionCount() const
{
    return static_cast<U32>(mConditions.size());
}

const AnimationStateMachine::Condition& AnimationStateMachine::GetCondition(U32 index) const
{
    assert(index < GetConditionCount());
    return mConditions[index];
}

U32 AnimationStateMachine::AddTransition(U32 fromState, U32 toState)
{
    mTransitions.emplace_back(fromState, toState);
    return GetTransitionCount() - 1;
}

void AnimationStateMachine::RemoveTransition(U32 index)
{
    assert(index < GetTransitionCount());
    // TODO : TODO
}

void AnimationStateMachine::ClearTransitions()
{
    // TODO : TODO
}

void AnimationStateMachine::SetTransitionFromState(U32 index, U32 fromState)
{
    assert(transitionIndex < GetTransitionCount());
    assert(fromState < GetStateCount());
    mTransitions[index].SetFromState(fromState);
}

void AnimationStateMachine::SetTransitionToState(U32 index, U32 toState)
{
    assert(transitionIndex < GetTransitionCount());
    assert(toState < GetStateCount());
    mTransitions[index].SetToState(toState);
}

void AnimationStateMachine::AddConditionToTransition(U32 transitionIndex, U32 conditionIndex)
{
    assert(transitionIndex < GetTransitionCount());
    assert(conditionIndex < GetConditionCount());
    mTransitions[transitionIndex].AddCondition(conditionIndex);
}

void AnimationStateMachine::RemoveConditionFromTransition(U32 transitionIndex, U32 conditionIndex)
{
    assert(transitionIndex < GetTransitionCount());
    assert(conditionIndex < GetConditionCount());
    // TODO : TODO
}

void AnimationStateMachine::ClearConditionsFromTransition(U32 transitionIndex)
{
    assert(transitionIndex < GetTransitionCount());
    // TODO : TODO
}

U32 AnimationStateMachine::GetTransitionCount() const
{
    return static_cast<U32>(mTransitions.size());
}

const AnimationStateMachine::Transition& AnimationStateMachine::GetTransition(U32 index) const
{
    assert(index < GetTransitionCount());
    return mTransitions[index];
}

} // namespace en