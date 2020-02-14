#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>

#if defined(ENLIVE_ENABLE_IMGUI)

#include <Enlivengine/Application/ImGuiToolManager.hpp>

#include <Enlivengine/Graphics/Animation.hpp>
#include <Enlivengine/Graphics/AnimationController.hpp>
#include <Enlivengine/Graphics/AnimationStateMachine.hpp>

#include <imgui-node-editor/imgui_node_editor.h>

namespace en
{

class ImGuiAnimationEditor : public ImGuiTool
{
	ENLIVE_SINGLETON(ImGuiAnimationEditor);
	~ImGuiAnimationEditor();

public:
	virtual ImGuiToolTab GetTab() const;
	virtual const char* GetName() const;

    virtual void Display();

	bool Initialize(AnimationStateMachinePtr stateMachinePtr);
	void Uninitialize();
	bool IsInitialized() const;

private:
	void LeftPanel(AnimationStateMachine& stateMachine);
	void Selection(AnimationStateMachine& stateMachine);
	void SelectedNode(AnimationStateMachine& stateMachine, ax::NodeEditor::NodeId node);
	void SelectedLink(AnimationStateMachine& stateMachine, ax::NodeEditor::LinkId link);
	void NewState(AnimationStateMachine& stateMachine);
	void NewParameter(AnimationStateMachine& stateMachine);
	void ParametersList(AnimationStateMachine& stateMachine);

	void NodeEditor(AnimationStateMachine& stateMachine);

	void AnimationStateMachineList();
	
private:
	AnimationStateMachinePtr mStateMachine;
    ax::NodeEditor::EditorContext* mEditorContext;

	// Left Panel elements
	static constexpr U32 kBufferSize{ 256 };
	char mStateInputTextBuffer[kBufferSize];
	char mParameterInputTextBuffer[kBufferSize];
};

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
