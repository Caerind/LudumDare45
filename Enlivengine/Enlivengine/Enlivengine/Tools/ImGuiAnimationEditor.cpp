#include <Enlivengine/Tools/ImGuiAnimationEditor.hpp>

#if defined(ENLIVE_ENABLE_IMGUI)

#include <imgui/imgui.h>

namespace en
{

ImGuiAnimationEditor::ImGuiAnimationEditor()
	: ImGuiTool()
{
    ax::NodeEditor::Config config;
    config.SettingsFile = "AnimationEditor.json";
    m_EditorContext = ax::NodeEditor::CreateEditor(&config);
}

ImGuiAnimationEditor::~ImGuiAnimationEditor()
{
    ax::NodeEditor::DestroyEditor(m_EditorContext);
}

ImGuiToolTab ImGuiAnimationEditor::GetTab() const
{
	return ImGuiToolTab::Main;
}

const char* ImGuiAnimationEditor::GetName() const
{
	return ICON_FA_LIST_UL " AnimationEditor";
}

void ImGuiAnimationEditor::Display()
{
    auto& io = ImGui::GetIO();

    ImGui::Text("FPS: %.2f (%.2gms)", io.Framerate, io.Framerate ? 1000.0f / io.Framerate : 0.0f);

    ImGui::Separator();

    ax::NodeEditor::SetCurrentEditor(m_EditorContext);
    ax::NodeEditor::Begin("My Editor", ImVec2(0.0, 0.0f));
        int uniqueId = 1;
        // Start drawing nodes.
        ax::NodeEditor::BeginNode(uniqueId++);
            ImGui::Text("Node A");
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Input);
                ImGui::Text("-> In");
            ax::NodeEditor::EndPin();
            ImGui::SameLine();
            ax::NodeEditor::BeginPin(uniqueId++, ax::NodeEditor::PinKind::Output);
                ImGui::Text("Out ->");
            ax::NodeEditor::EndPin();
        ax::NodeEditor::EndNode();
    ax::NodeEditor::End();
    ax::NodeEditor::SetCurrentEditor(nullptr);
}

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI