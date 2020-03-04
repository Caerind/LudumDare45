#include <Enlivengine/Application/ActionSystem.hpp>

#include <Enlivengine/System/Hash.hpp>

namespace en
{

ActionInput::ActionInput(std::string_view name)
    : mName(name)
    , mID(Hash::CRC32(name))
    , mActive(false) 
{
}

bool ActionInput::IsLogicalOperator() const
{
    return false;
}

std::string_view ActionInput::GetName() const
{
    return mName;
}

U32 ActionInput::GetID() const
{
    return mID;
}

bool ActionInput::IsActive() const
{
    return mActive;
}

void ActionInput::SetActive(bool active)
{
    mActive = active;
}

U32 ActionInput::GetPriorityLevel() const
{
    return 0;
}

ActionSystem::ActionSystem()
    : mEvents()
    , mInputs()
    , mDirty(false)
{
}

void ActionSystem::Update()
{
    if (mDirty)
    {
        Update_Internal();
        mDirty = false;
    }

    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (ActionInput* input = mInputs[i])
        {
            input->SetActive(input->IsCurrentlyActive(this));
        }
    }

    ClearEvents();
}

bool ActionSystem::IsInputActive(std::string_view inputName) const
{
    assert(IsInputExisting(inputName));
    return IsInputActive(Hash::CRC32(inputName));
}

bool ActionSystem::IsInputActive(U32 inputID) const
{
    assert(IsInputExisting(inputID));
    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (const ActionInput* input = mInputs[i])
        {
            if (input->GetID() == inputID)
            {
                return input->IsActive();
            }
        }
    }
    return false;
}

bool ActionSystem::IsInputExisting(std::string_view inputName) const
{
    return IsInputExisting(Hash::CRC32(inputName));
}

bool ActionSystem::IsInputExisting(U32 inputID) const
{
    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (mInputs[i] != nullptr && mInputs[i]->GetID() == inputID)
        {
            return true;
        }
    }
    return false;
}

U32 ActionSystem::GetInputCount() const
{
    return static_cast<U32>(mInputs.size());
}

const ActionInput* ActionSystem::GetInputByIndex(U32 index) const
{
    assert(index < GetInputCount());
    return mInputs[index];
}

const ActionInput* ActionSystem::GetInputByName(std::string_view inputName) const
{
    return GetInputByID(Hash::CRC32(inputName));
}

const ActionInput* ActionSystem::GetInputByID(U32 inputID) const
{
    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (mInputs[i] != nullptr && mInputs[i]->GetID() == inputID)
        {
            return mInputs[i];
        }
    }
    return nullptr;
}

void ActionSystem::ClearInputs()
{
    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (mInputs[i] != nullptr)
        {
            delete mInputs[i];
            mInputs[i] = nullptr;
        }
    }
    mInputs.clear();
}

void ActionSystem::AddEvent(const sf::Event& event)
{
    mEvents.push_back(event);
}

U32 ActionSystem::GetEventCount() const
{
    return static_cast<U32>(mEvents.size());
}

const sf::Event& ActionSystem::GetEvent(U32 index) const
{
    assert(index < GetEventCount());
    return mEvents[index];
}

void ActionSystem::ClearEvents()
{
    mEvents.clear();
}

void ActionSystem::AddInput_Internal(ActionInput* input)
{
    if (input != nullptr)
    {
        mInputs.push_back(input);
        mDirty = true;
    }
}

void ActionSystem::Update_Internal()
{
    bool priorityChanged = false;
    const U32 inputCount = GetInputCount();
    for (U32 i = 0; i < inputCount; ++i)
    {
        if (mInputs[i]->IsLogicalOperator())
        {
            if (ActionInputLogical* input = static_cast<ActionInputLogical*>(mInputs[i]))
            {
                const U32 previousPriority = input->GetPriorityLevel();
                const U32 currentPriority = GetMaxPriority(input->GetInputAID(), input->GetInputBID()) + 1;
                if (currentPriority != previousPriority)
                {
                    input->SetPriorityLevel(currentPriority);
                    priorityChanged = true;
                }
            }
        }
    }
    if (priorityChanged)
    {
        std::sort(mInputs.begin(), mInputs.end(), [](const ActionInput* a, const ActionInput* b)
        {
            assert(a != nullptr); //if (!a) return false;
            assert(b != nullptr); //if (!b) return true;
            const U32 pA = a->GetPriorityLevel();
            const U32 pB = b->GetPriorityLevel();
            if (pA == pB)
            {
                return a->GetID() < b->GetID();
            }
            else
            {
                return pA < pB;
            }
        });
    }
}

U32 ActionSystem::GetMaxPriority(U32 inputAID) const
{
    const ActionInput* inputA = GetInputByID(inputAID);
    return (inputA != nullptr) ? inputA->GetPriorityLevel() : 0;
}

U32 ActionSystem::GetMaxPriority(U32 inputAID, U32 inputBID) const
{
    const ActionInput* inputA = GetInputByID(inputAID);
    const U32 priorityA = (inputA != nullptr) ? inputA->GetPriorityLevel() : 0;
    const ActionInput* inputB = GetInputByID(inputBID);
    const U32 priorityB = (inputB != nullptr) ? inputB->GetPriorityLevel() : 0;
    return (priorityA >= priorityB) ? priorityA : priorityB;
}

ActionInputVariable::ActionInputVariable(std::string_view name, bool* variable) 
    : ActionInput(name)
    , mVariable(variable) 
{
}

ActionInputType ActionSystem::ActionInputVariable::GetInputType() const
{
    return ActionInputType::Variable;
}

bool ActionSystem::ActionInputVariable::IsCurrentlyActive(ActionSystem* system) const
{
    ENLIVE_UNUSED(system);
    if (mVariable != nullptr)
    {
        return *mVariable;
    }
    return false;
}

ActionSystem::ActionInputFunction::ActionInputFunction(std::string_view name, FuncType function)
    : ActionInput(name)
    , mFunction(function)
{
}

ActionInputType ActionSystem::ActionInputFunction::GetInputType() const
{
    return ActionInputType::Function;
}

bool ActionSystem::ActionInputFunction::IsCurrentlyActive(ActionSystem* system) const
{
    ENLIVE_UNUSED(system);
    if (mFunction)
    {
        return mFunction();
    }
    return false;
}

ActionSystem::ActionInputEvent::ActionInputEvent(std::string_view name, FuncType eventValidator)
    : ActionInput(name)
    , mEventValidator(eventValidator)
{
}

ActionInputType ActionSystem::ActionInputEvent::GetInputType() const
{
    return ActionInputType::Event;
}

bool ActionSystem::ActionInputEvent::IsCurrentlyActive(ActionSystem* system) const
{
    if (system != nullptr && mEventValidator)
    {
        const U32 eventCount = system->GetEventCount();
        for (U32 i = 0; i < eventCount; ++i)
        {
            if (mEventValidator(system->GetEvent(i)))
            {
                return true;
            }
        }
    }
    return false;
}

ActionSystem::ActionInputKey::ActionInputKey(std::string_view name, sf::Keyboard::Key key, ActionType actionType /*= ActionType::Pressed*/)
    : ActionInput(name)
    , mKey(key)
    , mActionType(actionType)
{
}

ActionInputType ActionSystem::ActionInputKey::GetInputType() const
{
    return ActionInputType::Key;
}

bool ActionSystem::ActionInputKey::IsCurrentlyActive(ActionSystem* system) const
{
    if (mActionType == ActionType::Hold)
    {
        return sf::Keyboard::isKeyPressed(mKey);
    }
    else if (system != nullptr)
    {
        const U32 eventCount = system->GetEventCount();
        for (U32 i = 0; i < eventCount; ++i)
        {
            const sf::Event& event = system->GetEvent(i);
            if (mActionType == ActionType::Pressed)
            {
                if (event.type == sf::EventType::KeyPressed && event.key.code == mKey)
                {
                    return true;
                }
            }
            else if (mActionType == ActionType::Released)
            {
                if (event.type == sf::EventType::KeyReleased && event.key.code == mKey)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

sf::Keyboard::Key ActionSystem::ActionInputKey::GetKey() const
{
    return mKey;
}

ActionType ActionSystem::ActionInputKey::GetType() const
{
    return mActionType;
}

void ActionSystem::ActionInputKey::SetKey(sf::Keyboard::Key key)
{
    mKey = key;
}

void ActionSystem::ActionInputKey::SetActionType(ActionType actionType)
{
    mActionType = actionType;
}

ActionSystem::ActionInputMouse::ActionInputMouse(std::string_view name, sf::Mouse::Button button, ActionType actionType /*= ActionType::Pressed*/)
    : ActionInput(name)
    , mButton(button)
    , mActionType(actionType)
{
}

ActionInputType ActionSystem::ActionInputMouse::GetInputType() const
{
    return ActionInputType::Mouse;
}

bool ActionSystem::ActionInputMouse::IsCurrentlyActive(ActionSystem* system) const
{
    if (mActionType == ActionType::Hold)
    {
        return sf::Mouse::isButtonPressed(mButton);
    }
    else if (system != nullptr)
    {
        const U32 eventCount = system->GetEventCount();
        for (U32 i = 0; i < eventCount; ++i)
        {
            const sf::Event& event = system->GetEvent(i);
            if (mActionType == ActionType::Pressed)
            {
                if (event.type == sf::EventType::MouseButtonPressed && event.mouseButton.button == mButton)
                {
                    return true;
                }
            }
            else if (mActionType == ActionType::Released)
            {
                if (event.type == sf::EventType::MouseButtonReleased && event.mouseButton.button == mButton)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

sf::Mouse::Button ActionSystem::ActionInputMouse::GetButton() const
{
    return mButton;
}

ActionType ActionSystem::ActionInputMouse::GetType() const
{
    return mActionType;
}

void ActionSystem::ActionInputMouse::SetButton(sf::Mouse::Button button)
{
    mButton = button;
}

void ActionSystem::ActionInputMouse::SetActionType(ActionType actionType)
{
    mActionType = actionType;
}

ActionSystem::ActionInputLogical::ActionInputLogical(std::string_view name, ActionInputLogicalOperator logic, U32 inputAID, U32 inputBID /*= U32_Max*/)
    : ActionInput(name)
    , mLogicOperator(logic)
    , mInputAID(inputAID)
    , mInputBID(inputBID)
    , mPriorityLevel(0)
{
}

ActionInputType ActionSystem::ActionInputLogical::GetInputType() const
{
    switch (GetLogicalOperator())
    {
    case ActionInputLogicalOperator::And: return ActionInputType::And;
    case ActionInputLogicalOperator::Or: return ActionInputType::Or;
    case ActionInputLogicalOperator::Not: return ActionInputType::Not;
    default: assert(false);
    }
    return ActionInputType::Not;
}

bool ActionSystem::ActionInputLogical::IsLogicalOperator() const
{
    return true;
}

bool ActionSystem::ActionInputLogical::IsCurrentlyActive(ActionSystem* system) const
{
    if (system != nullptr)
    {
        switch (mLogicOperator)
        {
        case ActionInputLogicalOperator::And: return system->GetInputByIndex(mInputAID)->IsActive() && system->GetInputByIndex(mInputBID)->IsActive();
        case ActionInputLogicalOperator::Or: return system->GetInputByIndex(mInputAID)->IsActive() || system->GetInputByIndex(mInputBID)->IsActive();
        case ActionInputLogicalOperator::Not: return !system->GetInputByIndex(mInputAID)->IsActive();
        default: assert(false);
        }
    }
    return false;
}

U32 ActionSystem::ActionInputLogical::GetPriorityLevel() const
{
    return mPriorityLevel;
}

ActionInputLogicalOperator ActionSystem::ActionInputLogical::GetLogicalOperator() const
{
    return mLogicOperator;
}

U32 ActionSystem::ActionInputLogical::GetInputAID() const
{
    return mInputAID;
}

U32 ActionSystem::ActionInputLogical::GetInputBID() const
{
    return mInputBID;
}

void ActionSystem::ActionInputLogical::SetInputAID(U32 inputID)
{
    mInputAID = inputID;
}

void ActionSystem::ActionInputLogical::SetInputBID(U32 inputID)
{
    mInputBID = inputID;
}

void ActionSystem::ActionInputLogical::SetPriorityLevel(U32 priorityLevel)
{
    mPriorityLevel = priorityLevel;
}

} // namespace en
