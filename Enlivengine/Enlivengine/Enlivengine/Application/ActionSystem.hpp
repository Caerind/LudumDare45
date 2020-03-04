#pragma once

#include <functional>
#include <string>

#include <Enlivengine/System/PrimitiveTypes.hpp>
#include <Enlivengine/System/Singleton.hpp>
#include <Enlivengine/System/NonCopyable.hpp>

#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

namespace en
{

enum class ActionInputType : U32
{
    Variable,
    Function,
    Event,
    Key,
    Mouse,
    And,
    Or,
    Not
};

enum class ActionType : U32
{
    Hold,
    Pressed,
    Released
};

enum class ActionInputLogicalOperator : U32
{
    And,
    Or,
    Not
};

class ActionSystem;

class ActionInput : private NonCopyable
{
public:
    ActionInput(std::string_view name);

    virtual ActionInputType GetInputType() const = 0;
    virtual bool IsLogicalOperator() const;

    std::string_view GetName() const;
    U32 GetID() const;
    bool IsActive() const;

    virtual bool IsCurrentlyActive(ActionSystem* system) const = 0;

protected:
    std::string_view mName;
    U32 mID;
    bool mActive;

private:
    friend class ActionSystem;
    void SetActive(bool active);
    virtual U32 GetPriorityLevel() const;
};

class ActionSystem : private NonCopyable
{
public:
    ActionSystem();

    void Update();

    bool IsInputActive(std::string_view inputName) const;
    bool IsInputActive(U32 inputID) const;

    bool IsInputExisting(std::string_view inputName) const;
    bool IsInputExisting(U32 inputID) const;

    template <typename ... Args>
    void AddInput(std::string_view name, ActionInputType type, Args&& ... args);
    U32 GetInputCount() const;
    const ActionInput* GetInputByIndex(U32 index) const;
    const ActionInput* GetInputByName(std::string_view inputName) const;
    const ActionInput* GetInputByID(U32 inputID) const;
    void ClearInputs();

    void AddEvent(const sf::Event& event);
    U32 GetEventCount() const;
    const sf::Event& GetEvent(U32 index) const;
    void ClearEvents();

private:
    std::vector<sf::Event> mEvents;
    std::vector<ActionInput*> mInputs;
    bool mDirty;

    void AddInput_Internal(ActionInput* input);
    void Update_Internal();
    U32 GetMaxPriority(U32 inputAID, U32 inputBID) const;    

private:
    class ActionInputVariable : public ActionInput
    {
    public:
        ActionInputVariable(std::string_view name, bool* variable);

        ActionInputType GetInputType() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;

    private:
        bool* mVariable;
    };

    class ActionInputFunction : public ActionInput
    {
    public:
        using FuncType = std::function<bool()>;

        ActionInputFunction(std::string_view name, FuncType function);
        
        ActionInputType GetInputType() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;

    private:
        FuncType mFunction;
    };

    class ActionInputEvent : public ActionInput
    {
    public:
        using FuncType = std::function<bool(const sf::Event& event)>;

        ActionInputEvent(std::string_view name, FuncType eventValidator);

        ActionInputType GetInputType() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;

    private:
        FuncType mEventValidator;
    };

    class ActionInputKey : public ActionInput
    {
    public:
        ActionInputKey(std::string_view name, sf::Keyboard::Key key, ActionType actionType = ActionType::Pressed);

        ActionInputType GetInputType() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;

        sf::Keyboard::Key GetKey() const;
        ActionType GetType() const;

        void SetKey(sf::Keyboard::Key key);
        void SetActionType(ActionType actionType);

    private:
        sf::Keyboard::Key mKey;
        ActionType mActionType;
    };

    class ActionInputMouse : public ActionInput
    {
    public:
        ActionInputMouse(std::string_view name, sf::Mouse::Button button, ActionType actionType = ActionType::Pressed);

        ActionInputType GetInputType() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;

        sf::Mouse::Button GetButton() const;
        ActionType GetType() const;

        void SetButton(sf::Mouse::Button button);
        void SetActionType(ActionType actionType);

    private:
        sf::Mouse::Button mButton;
        ActionType mActionType;
    };

    class ActionInputLogical : public ActionInput
    {
    public:
        ActionInputLogical(std::string_view name, ActionInputLogicalOperator logic, U32 inputAID, U32 inputBID = U32_Max);

        ActionInputType GetInputType() const override;
        bool IsLogicalOperator() const override;
        bool IsCurrentlyActive(ActionSystem* system) const override;
        U32 GetPriorityLevel() const override;

        ActionInputLogicalOperator GetLogicalOperator() const;
        U32 GetInputAID() const;
        U32 GetInputBID() const;

        void SetInputAID(U32 inputID);
        void SetInputBID(U32 inputID);
        void SetPriorityLevel(U32 priorityLevel);

    private:
        ActionInputLogicalOperator mLogicOperator;
        U32 mInputAID;
        U32 mInputBID;
        U32 mPriorityLevel;
    };
};

template <typename ... Args>
void ActionSystem::AddInput(std::string_view name, ActionInputType type, Args&& ... args)
{
    switch (type)
    {
    case ActionInputType::Variable: AddInput_Internal(new ActionInputVariable(name, std::forward<Args>(args)...)); break;
    case ActionInputType::Function: AddInput_Internal(new ActionInputFunction(name, std::forward<Args>(args)...)); break;
    case ActionInputType::Event: AddInput_Internal(new ActionInputEvent(name, std::forward<Args>(args)...)); break;
    case ActionInputType::Key: AddInput_Internal(new ActionInputKey(name, std::forward<Args>(args)...)); break;
    case ActionInputType::Mouse: AddInput_Internal(new ActionInputMouse(name, std::forward<Args>(args)...)); break;
    case ActionInputType::And: AddInput_Internal(new ActionInputLogical(name, ActionInputLogicalOperator::And, std::forward<Args>(args)...)); break;
    case ActionInputType::Or: AddInput_Internal(new ActionInputLogical(name, ActionInputLogicalOperator::Or, std::forward<Args>(args)...)); break;
    case ActionInputType::Not: AddInput_Internal(new ActionInputLogical(name, ActionInputLogicalOperator::Not, std::forward<Args>(args)...)); break;
    default: assert(false); break;
    }
}

} // namespace en
