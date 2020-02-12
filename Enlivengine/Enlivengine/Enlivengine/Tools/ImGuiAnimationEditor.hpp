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

private:
    ax::NodeEditor::EditorContext* m_EditorContext;
};

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
