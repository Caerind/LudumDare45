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

AnimationStateMachine::Condition::Condition(U32 parameterIndex)
{
    // TODO : No idea what to put here...
	mParameterIndex = parameterIndex;
    mOperator = Operator::Equal;
    mOperand.bValue = false;
}

AnimationStateMachine::Transition::Transition(U32 fromState, U32 toState)
	: mFromState(fromState)
	, mToState(toState)
{
}

U32 AnimationStateMachine::Transition::GetConditionCount() const
{
	return static_cast<U32>(mConditions.size());
}

void AnimationStateMachine::Transition::AddCondition(U32 conditionIndex)
{
	for (U32 condition : mConditions)
	{
		if (condition == conditionIndex)
		{
			return;
		}
	}
	mConditions.push_back(conditionIndex);
}

void AnimationStateMachine::Transition::RemoveCondition(U32 conditionIndex)
{
	const U32 conditionCount = GetConditionCount();
	for (U32 i = 0; i < conditionCount; ++i)
	{
		if (mConditions[i] == conditionIndex)
		{
			mConditions.erase(mConditions.begin() + i);
			return;
		}
	}
}

void AnimationStateMachine::Transition::ClearConditions()
{
	mConditions.clear();
}

AnimationStateMachine::AnimationStateMachine()
{
}

bool AnimationStateMachine::LoadFromFile(const std::string& filename)
{
    // TODO : TODO
	ENLIVE_UNUSED(filename);

	std::string str = filename.substr(0, filename.size() - 4);
	str += "json";

	mAnimation = ResourceManager::GetInstance().GetFromFilename<Animation>(str);

    return mAnimation.IsValid();
}

bool AnimationStateMachine::SaveToFile(const std::string& filename)
{
	// TODO : TODO
	ENLIVE_UNUSED(filename);
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
	assert(mAnimation.IsValid());
	assert(clipIndex < mAnimation.Get().GetClipCount());
	mStates.emplace_back(name, clipIndex);
	return GetStateCount() - 1;
}

void AnimationStateMachine::RemoveState(U32 index)
{
	assert(index < GetStateCount());
	mStates.erase(mStates.begin() + index);
}

void AnimationStateMachine::ClearStates()
{
	mStates.clear();
}

void AnimationStateMachine::SetStateName(U32 index, const std::string& name)
{
	assert(index < GetStateCount());
	mStates[index].SetName(name);
}

void AnimationStateMachine::SetStateClipIndex(U32 index, U32 clipIndex)
{
	assert(index < GetStateCount());
	assert(mAnimation.IsValid());
	assert(clipIndex < mAnimation.Get().GetClipCount());
	mStates[index].SetClipIndex(clipIndex);
}

void AnimationStateMachine::SetStateSpeedScale(U32 index, F32 speedScale)
{
	assert(index < GetStateCount());
	mStates[index].SetSpeedScale(speedScale);
}

void AnimationStateMachine::SetStateExitOnlyAtEnd(U32 index, bool exitOnlyAtEnd)
{
	assert(index < GetStateCount());
	mStates[index].SetExitOnlyAtEnd(exitOnlyAtEnd);
}

U32 AnimationStateMachine::GetStateCount() const
{
	return static_cast<U32>(mStates.size());
}

const AnimationStateMachine::State& AnimationStateMachine::GetState(U32 index) const
{
	assert(index < GetStateCount());
    return mStates[index];
}

U32 AnimationStateMachine::GetStateIndexByName(const std::string& name) const
{
	return GetStateIndexByName(Hash::CRC32(name.c_str()));
}

U32 AnimationStateMachine::GetStateIndexByName(U32 hashedName) const
{
	const U32 stateCount = GetStateCount();
	for (U32 i = 0; i < stateCount; ++i)
	{
		if (mStates[i].GetHashedName() == hashedName)
		{
			return i;
		}
	}
	return U32_Max;
}

U32 AnimationStateMachine::AddParameter(const std::string& name, Parameter::Type type)
{
	mParameters.emplace_back(name, type);
	return GetParameterCount() - 1;
}

void AnimationStateMachine::RemoveParameter(U32 index)
{
	assert(index < GetParameterCount());
	// TODO : TODO
}

void AnimationStateMachine::ClearParameters()
{
	// TODO : TODO
}

void AnimationStateMachine::SetParameterName(U32 index, const std::string& name)
{
	assert(index < GetParameterCount());
	mParameters[index].SetName(name);
}

void AnimationStateMachine::SetParameterType(U32 index, AnimationStateMachine::Parameter::Type type)
{
	assert(index < GetParameterCount());
	mParameters[index].SetType(type);
}

void AnimationStateMachine::SetParameterBoolean(U32 index, bool value)
{
	assert(index < GetParameterCount());
	assert(mParameters[index].GetType() == Parameter::Type::Boolean);
	mParameters[index].SetBooleanValue(value);
}

void AnimationStateMachine::SetParameterFloat(U32 index, F32 value)
{
	assert(index < GetParameterCount());
	assert(mParameters[index].GetType() == Parameter::Type::Float);
	mParameters[index].SetFloatValue(value);
}

void AnimationStateMachine::SetParameterInteger(U32 index, I32 value)
{
	assert(index < GetParameterCount());
	assert(mParameters[index].GetType() == Parameter::Type::Integer);
	mParameters[index].SetIntegerValue(value);
}

U32 AnimationStateMachine::GetParameterCount() const
{
	return static_cast<U32>(mParameters.size());
}

const AnimationStateMachine::Parameter& AnimationStateMachine::GetParameter(U32 index) const
{
	assert(index < GetParameterCount());
    return mParameters[index];
}

U32 AnimationStateMachine::GetParameterIndexByName(const std::string& name) const
{
	return GetParameterIndexByName(Hash::CRC32(name.c_str()));
}

U32 AnimationStateMachine::GetParameterIndexByName(U32 hashedName) const
{
	const U32 parameterCount = GetParameterCount();
	for (U32 i = 0; i < parameterCount; ++i)
	{
		if (mParameters[i].GetHashedName() == hashedName)
		{
			return i;
		}
	}
	return U32_Max;
}

U32 AnimationStateMachine::AddCondition(U32 parameterIndex)
{
    assert(parameterIndex < GetParameterCount());
    mConditions.emplace_back(parameterIndex);
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

	Parameter::Type type = mParameters[parameterIndex].GetType();
    switch (type)
    {
    case Parameter::Type::Boolean: mConditions[index].SetOperandBoolean(false); break;
    case Parameter::Type::Float: mConditions[index].SetOperandFloat(0.0f); break;
    case Parameter::Type::Integer: mConditions[index].SetOperandInteger(0); break;
    case Parameter::Type::Trigger: /* Nothing */ break;
    default: assert(false); break;
    }
}

void AnimationStateMachine::SetConditionOperator(U32 index, AnimationStateMachine::Condition::Operator operat)
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
        assert(operat == Condition::Operator::Equal || operat == Condition::Operator::NotEqual);
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
    assert(index < GetTransitionCount());
    assert(fromState < GetStateCount());
    mTransitions[index].SetFromState(fromState);
}

void AnimationStateMachine::SetTransitionToState(U32 index, U32 toState)
{
    assert(index < GetTransitionCount());
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
	mTransitions[transitionIndex].RemoveCondition(conditionIndex);
}

void AnimationStateMachine::ClearConditionsFromTransition(U32 transitionIndex)
{
    assert(transitionIndex < GetTransitionCount());
	mTransitions[transitionIndex].ClearConditions();
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