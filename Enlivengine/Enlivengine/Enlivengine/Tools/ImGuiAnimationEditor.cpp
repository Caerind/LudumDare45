#include <Enlivengine/Tools/ImGuiAnimationEditor.hpp>

#if defined(ENLIVE_ENABLE_IMGUI)

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Enlivengine/Tools/ImGuiHelper.hpp>

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
	if (IsInitialized() && mStateMachine.IsValid())
	{
		AnimationStateMachine& stateMachine = mStateMachine.Get();
		ax::NodeEditor::SetCurrentEditor(mEditorContext);

		ImGui::Columns(2, "animEditorColumns");

		LeftPanel(stateMachine);
		ImGui::NextColumn();

		NodeEditor(stateMachine);
		ImGui::NextColumn();

		ImGui::Columns(1);

		ax::NodeEditor::SetCurrentEditor(nullptr);
	}
	else
	{
		AnimationStateMachineList();
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
		AnimationStateMachine& stateMachine = stateMachinePtr.Get();
		if (!stateMachine.GetAnimation().IsValid())
		{
			return false;
		}

		// Debug
		{
			stateMachine.AddState("TestA", 0);
			stateMachine.AddState("TestB", 1);
			stateMachine.AddState("TestC", 2);
			stateMachine.AddState("TestD", 3);

			stateMachine.AddTransition(0, 1);
			stateMachine.AddTransition(0, 1); // TODO : See many links
			stateMachine.AddTransition(0, 2);
			stateMachine.AddTransition(2, 3);
			stateMachine.AddTransition(3, 0);

			stateMachine.AddParameter("Jumping", AnimationStateMachine::Parameter::Type::Boolean);
			stateMachine.AddParameter("Speed", AnimationStateMachine::Parameter::Type::Float);
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

void ImGuiAnimationEditor::LeftPanel(AnimationStateMachine& stateMachine)
{
	static bool initialLeftColSize = true;
	if (initialLeftColSize)
	{
		ImGui::PushItemWidth(100.0f);
	}

	Selection(stateMachine);
	NewState(stateMachine);
	NewParameter(stateMachine);
	ParametersList(stateMachine);

	if (initialLeftColSize)
	{
		ImGui::PopItemWidth();
		initialLeftColSize = false;
	}
}

void ImGuiAnimationEditor::Selection(AnimationStateMachine& stateMachine)
{
	const U32 selected = static_cast<U32>(ax::NodeEditor::GetSelectedObjectCount());
	std::vector<ax::NodeEditor::NodeId> selectedNodes;
	std::vector<ax::NodeEditor::LinkId> selectedLinks;
	selectedNodes.resize(selected);
	selectedLinks.resize(selected);
	const U32 nodeCount = static_cast<U32>(ax::NodeEditor::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size())));
	const U32 linkCount = static_cast<U32>(ax::NodeEditor::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size())));
	if (nodeCount >= 1)
	{
		SelectedNode(stateMachine, selectedNodes[0]);
	}
	else if (linkCount >= 1)
	{
		SelectedLink(stateMachine, selectedLinks[0]);
	}
	else
	{
		ImGui::CollapsingHeader("Empty selection");
	}
}

void ImGuiAnimationEditor::SelectedNode(AnimationStateMachine& stateMachine, ax::NodeEditor::NodeId node)
{
	ImGui::CollapsingHeader("Selected State");
	ImGui::Indent();

	const int stateUID = 1;
	const std::uintptr_t nodeID = reinterpret_cast<std::uintptr_t>(node.AsPointer());
	const U32 stateIndex = static_cast<U32>(nodeID - stateUID);

	ImGui::PushID(stateIndex);
	const AnimationStateMachine::State& state = stateMachine.GetState(stateIndex);

	float speedScale = static_cast<float>(state.GetSpeedScale());
	ImGui::PushItemWidth(90.0f);
	if (ImGui::InputFloat("SpeedScale##SelectedNode", &speedScale))
	{
		stateMachine.SetStateSpeedScale(stateIndex, static_cast<F32>(speedScale));
	}
	ImGui::PopItemWidth();

	bool exitOnlyAtEnd = state.GetExitOnlyAtEnd();
	if (ImGui::Checkbox("ExitOnlyAtEnd##SelectedNode", &exitOnlyAtEnd))
	{
		stateMachine.SetStateExitOnlyAtEnd(stateIndex, exitOnlyAtEnd);
	}
	ImGui::PopID();

	ImGui::Unindent();
}

void ImGuiAnimationEditor::SelectedLink(AnimationStateMachine& stateMachine, ax::NodeEditor::LinkId link)
{
	ImGui::CollapsingHeader("Selected Transition");
	ImGui::Indent();

	const int transitionUID = 1000 + 1;
	const std::uintptr_t linkID = reinterpret_cast<std::uintptr_t>(link.AsPointer());
	const U32 transitionIndex = static_cast<U32>(linkID - transitionUID);

	const AnimationStateMachine::Transition& transition = stateMachine.GetTransition(transitionIndex);

	const AnimationStateMachine::State& fromState = stateMachine.GetState(transition.GetFromState());
	const AnimationStateMachine::State& toState = stateMachine.GetState(transition.GetToState());

	ImGui::Text("From: %s", fromState.GetName().c_str());
	ImGui::Text("To: %s", toState.GetName().c_str());

	ImGui::Unindent();
}

void ImGuiAnimationEditor::NewState(AnimationStateMachine& stateMachine)
{
	if (ImGui::CollapsingHeader("New state"))
	{
		ImGui::Indent();

		bool validNewState = true;
		ImGui::InputText("Name##NewState", mStateInputTextBuffer, kBufferSize);
		if (strlen(mStateInputTextBuffer) <= 0)
		{
			validNewState = false;
		}

		static int stateClipIndex = 0;

		if (stateMachine.GetAnimation().IsValid() && stateMachine.GetAnimation().Get().GetClipCount() > 0)
		{
			const Animation& animation = stateMachine.GetAnimation().Get();
			ImGui::InputInt("##NewStateClipIndex", &stateClipIndex);
			if (stateClipIndex >= static_cast<int>(animation.GetClipCount()))
			{
				stateClipIndex = 0;
			}
			if (stateClipIndex < 0)
			{
				stateClipIndex = static_cast<int>(animation.GetClipCount() - 1);
			}

			const Animation::Clip& clip = animation.GetClip(static_cast<U32>(stateClipIndex));
			if (clip.GetFrameCount() > 0)
			{
				static Time accumulator;
				static U32 clipFrameIndex = 0;
				if (clipFrameIndex >= clip.GetFrameCount())
				{
					clipFrameIndex = 0;
				}
				accumulator += seconds(ImGui::GetIO().DeltaTime);
				const U32 frameIndex = clip.GetFrameIndex(clipFrameIndex);
				if (frameIndex < animation.GetFrameCount())
				{
					const Animation::Frame& frame = animation.GetFrame(frameIndex);
					assert(animation.GetTexture().IsValid()); // lazy to check
					const Texture& texture = animation.GetTexture().Get();
					ImGui::PreviewTexture(texture, frame.GetRect(), 100.0f, true);

					if (accumulator > frame.GetDuration())
					{
						accumulator -= frame.GetDuration();
						clipFrameIndex++;
					}
				}
				else
				{
					ImGui::TextColored(Color::Orange.toImGuiColor(), "Invalid clip/frame");
					validNewState = false;
				}
			}
			else
			{
				ImGui::TextColored(Color::Orange.toImGuiColor(), "Invalid clip");
				validNewState = false;
			}
		}
		else
		{
			validNewState = false;
		}
		if (validNewState && (stateClipIndex >= static_cast<int>(stateMachine.GetAnimation().Get().GetClipCount()) || stateClipIndex < 0))
		{
			validNewState = false;
		}

		if (validNewState)
		{
			if (ImGui::Button("Add##NewState"))
			{
				stateMachine.AddState(std::string(mStateInputTextBuffer), stateClipIndex);
#ifdef ENLIVE_COMPILER_MSVC
				strcpy_s(mStateInputTextBuffer, "");
#else
				strcpy(mStateInputTextBuffer, "");
#endif // ENLIVE_COMPILER_MSVC
			}
		}
		else
		{
			ImGui::DisabledButton("Add##NewStateDisabled");
		}

		ImGui::Unindent();
	}
}

void ImGuiAnimationEditor::NewParameter(AnimationStateMachine& stateMachine)
{
	if (ImGui::CollapsingHeader("New parameter"))
	{
		ImGui::Indent();

		bool validNewParameter = true;

		// Name
		ImGui::InputText("Name##NewParameter", mParameterInputTextBuffer, kBufferSize);
		if (strlen(mParameterInputTextBuffer) <= 0)
		{
			validNewParameter = false;
		}

		static int parameterType = 0;
		static bool boolValue = true;
		static F32 floatValue = 0.0f;
		static I32 intValue = 0;

		// Type
		static const char* paramTypes[] = { "Boolean", "Float", "Integer", "Trigger" };
		assert(IM_ARRAYSIZE(paramTypes) == static_cast<int>(AnimationStateMachine::Parameter::Type::Count));
		int previousType = parameterType;
		if (ImGui::Combo("Type##NewParameter", &parameterType, paramTypes, IM_ARRAYSIZE(paramTypes)))
		{
			if (previousType != parameterType)
			{
				boolValue = true;
				floatValue = 0.0f;
				intValue = 0;
			}
		}

		// Value
		ImGui::Indent();
		switch (parameterType)
		{
		case 0: // Boolean
		{
			static const char* boolValues[] = { "False", "True" };
			int boolValueIndex = (int)boolValue;
			if (ImGui::Combo("##NewParameterBoolParameterInput", &boolValueIndex, boolValues, IM_ARRAYSIZE(boolValues)))
			{
				boolValue = (bool)boolValueIndex;
			}
		} break;
		case 1: // Float
			ImGui::InputFloat("##NewParameterFloatParameterInput", &floatValue); 
			break;
		case 2: // Integer
			ImGui::InputInt("##NewParameterIntParameterInput", &intValue);
			break;
		case 3: // Trigger
			break;
		default: 
			break;
		}
		ImGui::Unindent();

		if (validNewParameter)
		{
			if (ImGui::Button("Add##NewParameter"))
			{
				const U32 parameterIndex = stateMachine.AddParameter(std::string(mParameterInputTextBuffer), static_cast<AnimationStateMachine::Parameter::Type>(parameterType));
				switch (parameterType)
				{
				case 0: stateMachine.SetParameterBoolean(parameterIndex, boolValue); break;
				case 1: stateMachine.SetParameterFloat(parameterIndex, floatValue); break;
				case 2: stateMachine.SetParameterInteger(parameterIndex, intValue); break;
				case 3: break;
				default: break;
				}
				parameterType = 0;
				boolValue = true;
				floatValue = 0.0f;
				intValue = 0;
#ifdef ENLIVE_COMPILER_MSVC
				strcpy_s(mParameterInputTextBuffer, "");
#else
				strcpy(mParameterInputTextBuffer, "");
#endif // ENLIVE_COMPILER_MSVC
			}
		}
		else
		{
			ImGui::DisabledButton("Add##NewParameterDisabled");
		}

		ImGui::Unindent();
	}
}

void ImGuiAnimationEditor::ParametersList(AnimationStateMachine& stateMachine)
{
	if (ImGui::CollapsingHeader("Parameters list"))
	{
		ImGui::Indent();

		const U32 parameterCount = stateMachine.GetParameterCount();
		for (U32 parameterIndex = 0; parameterIndex < parameterCount; ++parameterIndex)
		{
			const AnimationStateMachine::Parameter& parameter = stateMachine.GetParameter(parameterIndex);

			ImGui::Text("-");
			ImGui::SameLine();
			
			ImVec4 color;
			switch (parameter.GetType())
			{
			case AnimationStateMachine::Parameter::Type::Boolean: color = Color::Lime.toImGuiColor(); break;
			case AnimationStateMachine::Parameter::Type::Float: color = Color::Salmon.toImGuiColor(); break;
			case AnimationStateMachine::Parameter::Type::Integer: color = Color::Cyan.toImGuiColor(); break;
			case AnimationStateMachine::Parameter::Type::Trigger: color = Color::Yellow.toImGuiColor(); break;
			default: break;
			}
			ImGui::TextColored(color, parameter.GetName().c_str());
			ImGui::SameLine();

			if (parameter.GetType() < AnimationStateMachine::Parameter::Type::Trigger)
			{
				ImGui::Text(" : ");
				ImGui::SameLine();

				ImGui::PushID(parameterIndex);
				ImGui::PushItemWidth(80.0f);
				switch (parameter.GetType())
				{
				case AnimationStateMachine::Parameter::Type::Boolean: // Boolean
				{
					static const char* boolValues[] = { "False", "True" };
					int boolValueIndex = (int)parameter.GetBooleanValue();
					if (ImGui::Combo("##ParametersListBoolParametersListInput", &boolValueIndex, boolValues, IM_ARRAYSIZE(boolValues)))
					{
						stateMachine.SetParameterBoolean(parameterIndex, (bool)boolValueIndex);
					}
				} break;
				case AnimationStateMachine::Parameter::Type::Float: // Float
				{
					float floatValue = parameter.GetFloatValue();
					if (ImGui::InputFloat("##ParametersListFloatParametersListInput", &floatValue))
					{
						stateMachine.SetParameterFloat(parameterIndex, (F32)floatValue);
					}
				} break;
				case AnimationStateMachine::Parameter::Type::Integer: // Integer
				{
					int intValue = parameter.GetIntegerValue();
					if (ImGui::InputInt("##ParametersListIntParametersListInput", &intValue))
					{
						stateMachine.SetParameterInteger(parameterIndex, (I32)intValue);
					}
				} break;
				default:
					break;
				}
				ImGui::PopItemWidth();
				ImGui::PopID();
			}

		}

		ImGui::Unindent();
	}
}

void ImGuiAnimationEditor::NodeEditor(AnimationStateMachine& stateMachine)
{
	// TODO : Make those private constexpr static
	const int stateUID = 1;
	const int transitionUID = 1000 + 1;
	const int transitionInputUID = 2000 + 1;
	const int transitionOutputUID = 3000 + 1;

	const float paddingX = 12.0f;
	const float paddingY = 6.0f;
	const float rounding = 4.0f;

	ax::NodeEditor::Begin(stateMachine.GetIdentifier().c_str(), ImVec2(0.0f, 0.0f));

	const U32 stateCount = stateMachine.GetStateCount();
	for (U32 stateIndex = 0; stateIndex < stateCount; ++stateIndex)
	{
		int stateID = static_cast<int>(stateIndex);
		const AnimationStateMachine::State& state = stateMachine.GetState(stateIndex);

		ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBg, ImColor(128, 128, 128, 250));
		ax::NodeEditor::PushStyleColor(ax::NodeEditor::StyleColor_NodeBorder, ImColor(32, 32, 32, 250));

		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodePadding, ImVec4(paddingX, paddingY, paddingX, paddingY));
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeRounding, rounding);
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_NodeBorderWidth, 2.0f);
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_LinkStrength, 0.0f);
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PivotSize, ImVec2(0.0f, 0.0f));
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinCorners, 12);
		ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinRadius, paddingX * 2.0f);
		ax::NodeEditor::BeginNode(stateUID + stateID);

		ImGui::Text(state.GetName().c_str());
		ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());

		const U32 transitionCount = stateMachine.GetTransitionCount();
		for (U32 j = 0; j < transitionCount; ++j)
		{
			int transitionID = static_cast<int>(j);
			const AnimationStateMachine::Transition& transition = stateMachine.GetTransition(j);
			if (transition.GetToState() == stateIndex) // Input
			{
				ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowSize, 10.0f);
				ax::NodeEditor::PushStyleVar(ax::NodeEditor::StyleVar_PinArrowWidth, 10.0f);
				ax::NodeEditor::BeginPin(transitionInputUID + transitionID, ax::NodeEditor::PinKind::Input);
				ax::NodeEditor::PinRect(nodeRect.GetTL(), nodeRect.GetBR());
				ax::NodeEditor::EndPin();
				ax::NodeEditor::PopStyleVar(2);
			}
			if (transition.GetFromState() == stateIndex) // Output
			{
				ImRect outputsRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
				ax::NodeEditor::BeginPin(transitionOutputUID + transitionID, ax::NodeEditor::PinKind::Output);
				ax::NodeEditor::PinRect(nodeRect.GetTL(), nodeRect.GetBR());
				ax::NodeEditor::EndPin();
			}
		}

		ax::NodeEditor::EndNode();
		ax::NodeEditor::PopStyleVar(7);
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
}

void ImGuiAnimationEditor::AnimationStateMachineList()
{
	// TODO : AnimationStateMachine list
	ImGui::Text("In future update, we will have the list of \nall the AnimationStateMachine here\n\nBut unfortunatelly, you will have to wait\nand use the ResourceBrowser...\n\nSorry for the incovenience.");
}

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI