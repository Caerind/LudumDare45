#pragma once

#include <Enlivengine/Application/ResourceManager.hpp>
#include <Enlivengine/Graphics/Animation.hpp>

namespace en
{

class AnimationStateMachine : public Resource<AnimationStateMachine>
{
public:

	/*

	enum class ParameterType
	{
		Boolean,
		Float,
		Integer,
		Trigger
	};

	// TODO : Make more private later
	class State
	{
	public:
		State()
		{
			name = "";
			hashedName = 0;
			clipIndex = 0;
			speedScale = 1.0f;
			exitOnlyAtEnd = false;
		}

		std::string name;
		U32 hashedName;
		U32 clipIndex;
		F32 speedScale;
		bool exitOnlyAtEnd;
	};

	class Parameter
	{
	public:
		Parameter(const std::string& name, ParameterType type);

		const std::string& GetName() const { return mName; }
		U32 GetHashedName() const { return mHashedName; }

		void SetType(ParameterType type);
		ParameterType GetType() const { return mType; }

		void SetBooleanValue(bool value);
		void SetFloatValue(F32 value);
		void SetIntegerValue(I32 value);
		void SetTriggerValue(bool value);
		bool GetBooleanValue() const;
		F32 GetFloatValue() const;
		I32 GetIntegerValue() const;
		bool GetTriggerValue() const;

	private:
		std::string mName;
		U32 mHashedName;
		ParameterType mType;
		union
		{
			bool bValue;
			F32 fValue;
			I32 iValue;
		} mValue;
	};

	class Condition
	{
	public:
		enum class Operator
		{
			Equal,
			NotEqual,
			Less,
			LessEq,
			Greater,
			GreaterEq
		};

		Condition()
		{
			mParameterID = 0;
			mOperator = Operator::Equal;
			mOperand.bValue = true;
		}

		void SetParameterID(U32 parameterID) { mParameterID = parameterID; }
		U32 GetParameterID() const { return mParameterID; }

		Operator GetOperator() const { return mOperator; }
		void SetOperator(Operator op) { mOperator = op; }

		bool GetOperandBoolean() const;
		F32 GetOperandFloat() const;
		I32 GetOperandInteger() const;
		void SetOperandBoolean(bool value);
		void SetOperandFloat(F32 value);
		void SetOperandInteger(I32 value);

	private:
		U32 mParameterID;
		Operator mOperator;
		union
		{
			bool bValue;
			F32 fValue;
			I32 iValue;
		} mOperand;
	};

	// TODO : Make more private later
	class Transition
	{
	public:
		Transition()
		{
			from = 0;
			to = 0;
		}

		U32 from; // HashedName, as Index is more subject to change
		U32 to; // HashedName, as Index is more subject to change
		std::vector<U32> mConditions;
	};

public:
	AnimationStateMachine();

	// IO
	bool LoadFromFile(const std::string& filename);
	bool SaveToFile(const std::string& filename);

	// Animation
	void SetAnimation(AnimationPtr animation);
	const AnimationPtr& GetAnimation() const;

	// States
	U32 AddState(const std::string& name, U32 clipIndex);
	void RemoveState(U32 index);
	void ClearStates();
	void SetStateName(U32 index, const std::string& name);
	void SetStateClipIndex(U32 index, U32 clipIndex);
	void SetStateSpeedScale(U32 index, F32 speedScale);
	void SetStateExitOnlyAtEnd(U32 index, bool exitOnlyAtEnd);
	U32 GetStateCount() const;
	const AnimationStateMachine::State& GetState(U32 index) const;
	U32 GetStateIndexByName(const std::string& name) const;
	U32 GetStateIndexByName(U32 hashedName) const;

	// Parameters
	U32 AddParameter(const std::string& name, ParameterType type);
	void RemoveParameter(U32 index);
	void ClearParameters();
	void SetParameterName(U32 index, const std::string& name);
	void SetParameterType(U32 index, ParameterType type);
	void SetParameterBoolean(U32 index, bool value);
	void SetParameterFloat(U32 index, F32 value);
	void SetParameterInteger(U32 index, I32 value);
	U32 GetParameterCount() const;
	const AnimationStateMachine::Parameter& GetParameter(U32 index) const;
	U32 GetParameterIndexByName(const std::string& name) const;
	U32 GetParameterIndexByName(U32 hashedName) const;

	U32 AddCondition(U32 parameterIndex);
	U32 GetConditionCount() const;
	const AnimationStateMachine::Condition& GetCondition(U32 index) const;
	void RemoveCondition(U32 index);
	void ClearConditions();

	U32 AddTransition(U32 fromState, U32 toState);
	U32 GetTransitionCount() const;
	const AnimationStateMachine::Transition& GetTransition(U32 index) const;
	void RemoveTransition(U32 index);
	void ClearTransitions();

private:
	AnimationPtr mAnimation;
	std::vector<State> mStates;
	std::vector<Parameter> mParameters;
	std::vector<Condition> mConditions;
	std::vector<Transition> mTransitions;


*/
};

using AnimationStateMachinePtr = ResourcePtr<AnimationStateMachine>;

class AnimationStateMachineLoader
{
public:
	AnimationStateMachineLoader() = delete;
	~AnimationStateMachineLoader() = delete;

	static ResourceLoader<AnimationStateMachine> FromFile(const std::string& filename)
	{
		return ResourceLoader<AnimationStateMachine>([&filename](AnimationStateMachine& r)
		{
			const bool result = false; /* r.LoadFromFile(filename); */ // TODO : Don't miss me :)
			r.mFilename = (result) ? filename : "";
			return result;
		});
	}
};

} // namespace en
