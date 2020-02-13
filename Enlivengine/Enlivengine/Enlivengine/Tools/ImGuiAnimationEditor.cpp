#include <Enlivengine/Tools/ImGuiAnimationEditor.hpp>

#if defined(ENLIVE_ENABLE_IMGUI)

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

namespace en
{

ImGuiAnimationEditor::ImGuiAnimationEditor()
	: ImGuiTool()
	, mStateMachine()
	, mEditorContext(nullptr)
{
}

ImGuiAnimationEditor::~ImGuiAnimationEditor()
{
	// We won't be able to save here as the resource might have 
	// been freed before by the ResourceManager.
	// We can't neither test the IsInitialized as it requires the 
	// ResourceManager to be valid and we can't be sure about it.

	mStateMachine = AnimationStateMachinePtr();
	if (mEditorContext != nullptr)
	{
		ax::NodeEditor::DestroyEditor(mEditorContext);
	}
}

ImGuiToolTab ImGuiAnimationEditor::GetTab() const
{
	return ImGuiToolTab::Main;
}

const char* ImGuiAnimationEditor::GetName() const
{
	return ICON_FA_RUNNING " AnimationEditor";
}

void ImGuiAnimationEditor::Display()
{
	if (IsInitialized())
	{
		AnimationStateMachine& stateMachine = mStateMachine.Get();
		ax::NodeEditor::SetCurrentEditor(mEditorContext);
        ax::NodeEditor::Begin(stateMachine.GetIdentifier().c_str(), ImVec2(0.0f, 0.0f));

        const int stateUID = 1;
        const int transitionUID = 1000 + 1;
        const int transitionInputUID = 2000 + 1;
        const int transitionOutputUID = 3000 + 1;

        // Debug
		static bool once = true;
		if (once)
		{
			stateMachine.AddState("TestA", 0);
			stateMachine.AddState("TestB", 1);
			stateMachine.AddState("TestC", 2);
			stateMachine.AddState("TestD", 3);

			stateMachine.AddTransition(0, 1);
			stateMachine.AddTransition(0, 1);
			stateMachine.AddTransition(0, 1);
			stateMachine.AddTransition(0, 2);
			stateMachine.AddTransition(2, 3);
			stateMachine.AddTransition(3, 0);

			once = false;
        }

		const U32 stateCount = stateMachine.GetStateCount();
		for (U32 i = 0; i < stateCount; ++i)
		{
			int stateID = static_cast<int>(i);
			const AnimationStateMachine::State& state = stateMachine.GetState(i);

			const float paddingX = 12.0f;
			const float paddingY = 6.0f;
            const float rounding = 4.0f;

			ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(128, 128, 128, 255));
			ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));

			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodePadding, ImVec4(paddingX, paddingY, paddingX, paddingY));
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, rounding);
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
			ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 0.0f);
            ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotSize, ImVec2(0.0f, 0.0f));
            ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, 0.0f);
            ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, 0.0f);
			ax::NodeEditor::BeginNode(stateUID + stateID);

            ImGui::Text(state.GetName().c_str());
            ImRect rect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            rect.Min.x -= paddingX;
            rect.Min.y -= paddingY;
            rect.Max.x += paddingX;
            rect.Max.y += paddingY;

			const U32 transitionCount = stateMachine.GetTransitionCount();
			for (U32 j = 0; j < transitionCount; ++j)
			{
				int transitionID = static_cast<int>(j);
				const AnimationStateMachine::Transition& transition = stateMachine.GetTransition(j);
				if (transition.GetToState() == i) // Input
				{
                    ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 10.0f);
                    ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 10.0f);
					ax::NodeEditor::BeginPin(transitionInputUID + transitionID, ax::NodeEditor::PinKind::Input);
					ax::NodeEditor::PinRect(rect.GetTL(), rect.GetBR());
                    ax::NodeEditor::EndPin();
                    ax::NodeEditor::PopStyleVar(2);
				}
				if (transition.GetFromState() == i) // Output
				{
					ImRect outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
					ax::NodeEditor::BeginPin(transitionOutputUID + transitionID, ax::NodeEditor::PinKind::Output);
					ax::NodeEditor::PinRect(rect.GetTL(), rect.GetBR());
					ax::NodeEditor::EndPin();
				}
            }

			ax::NodeEditor::EndNode();
			ax::NodeEditor::PopStyleVar(8);
			ax::NodeEditor::PopStyleColor(2);
		}

		const U32 transitionCount = stateMachine.GetTransitionCount();
		for (U32 i = 0; i < transitionCount; ++i)
		{
			int transitionID = static_cast<int>(i);

            ax::NodeEditor::Link(
                transitionUID + transitionID,
                transitionInputUID + transitionID,
                transitionOutputUID + transitionID,
                Color::Lime.toImGuiColor(),
                2.0f
            );
		}

		ax::NodeEditor::End();
		ax::NodeEditor::SetCurrentEditor(nullptr);
	}
	else
	{
		// TODO : AnimationStateMachine list
		ImGui::Text("In future update, we will have the list of \nall the AnimationStateMachine here\n\nBut unfortunatelly, you will have to wait\nand use the ResourceBrowser...\n\nSorry for the incovenience.");
	}
}

bool ImGuiAnimationEditor::Initialize(AnimationStateMachinePtr stateMachinePtr)
{
	if (IsInitialized())
	{
		Uninitialize();
	}

	if (stateMachinePtr.IsValid())
	{
		const AnimationStateMachine& stateMachine = stateMachinePtr.Get();
		if (!stateMachine.GetAnimation().IsValid())
		{
			return false;
		}

		mStateMachine = stateMachinePtr;

		ax::NodeEditor::Config config;
		config.SettingsFile = "AnimationEditor.json";
		mEditorContext = ax::NodeEditor::CreateEditor(&config);

		mVisible = true;
		AskForFocus();

		return true;
	}

	return false;
}

void ImGuiAnimationEditor::Uninitialize()
{
	if (IsInitialized())
	{
		// TODO : Save ?

		mStateMachine = AnimationStateMachinePtr();
		ax::NodeEditor::DestroyEditor(mEditorContext);

		mVisible = false;
	}
}

bool ImGuiAnimationEditor::IsInitialized() const
{
	return mStateMachine.IsValid() && mEditorContext != nullptr;
}

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI