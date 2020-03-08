#include <Enlivengine/Tools/ImGuiInputEditor.hpp>

#if defined(ENLIVE_ENABLE_IMGUI)

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <Enlivengine/Tools/ImGuiHelper.hpp>
#include <Enlivengine/System/Assert.hpp>
#include <Enlivengine/Application/Application.hpp>
#include <Enlivengine/Application/ActionSystem.hpp>

namespace en
{

ImGuiInputEditor::ImGuiInputEditor()
	: ImGuiTool()
{
}

ImGuiInputEditor::~ImGuiInputEditor()
{
}

ImGuiToolTab ImGuiInputEditor::GetTab() const
{
	return ImGuiToolTab::Main;
}

const char* ImGuiInputEditor::GetName() const
{
	return ICON_FA_GAMEPAD " InputEditor";
}

void ImGuiInputEditor::Display()
{
	static constexpr U32 kBufferSize{ 256 };

	static const char* actionInputTypeNames[] = { 
		"Variable", 
		"Function", 
		"Event", 
		"Key",
		"Mouse",
		"And",
		"Or",
		"Not"
	};
	assert(IM_ARRAYSIZE(actionInputTypeNames) == static_cast<int>(ActionInputType::Count));

	static const char* actionTypeNames[] = {
		"Hold",
		"Pressed",
		"Released"
	};
	assert(IM_ARRAYSIZE(actionTypeNames) == static_cast<int>(ActionType::Count)); 
	
	static const char* actionInputLogicalOperatorNames[] = {
		"And",
		"Or",
		"Not"
	};
	assert(IM_ARRAYSIZE(actionInputLogicalOperatorNames) == static_cast<int>(ActionInputLogicalOperator::Count));

	static const char* mouseButtonNames[] = {
		"Left",
		"Right",
		"Middle",
		"XButton1",
		"XButton2"
	};
	assert(IM_ARRAYSIZE(mouseButtonNames) == static_cast<int>(sf::Mouse::Button::ButtonCount));

	ActionSystem& actionSystem = Application::GetInstance().GetActionSystem();

	// New action input
	{
		static char newActionInputName[kBufferSize];
		static int newActionInputType = 0;
		static int newActionInputActionType = 0;
		static int newActionInputKey = 0;
		static int newActionInputButton = 0;
		static int newActionInputAIndex = 0;
		static int newActionInputBIndex = 0;

		ImGui::InputText("Name##NewActionInput", newActionInputName, kBufferSize);
		ImGui::Combo("Type##NewActionInput", &newActionInputType, actionInputTypeNames, IM_ARRAYSIZE(actionInputTypeNames));

		const ActionInputType actionInputType = static_cast<ActionInputType>(newActionInputType);
		switch (actionInputType)
		{
		case ActionInputType::Variable: ImGui::Text("Not configurable yet"); /* TODO_Someday */ break;
		case ActionInputType::Function: ImGui::Text("Not configurable yet"); /* TODO_Someday */ break;
		case ActionInputType::Event: ImGui::Text("Not configurable yet"); /* TODO_Someday */ break;
		case ActionInputType::Key: 
		{
			ImGui::Combo("ActionType##NewActionInput", &newActionInputActionType, actionTypeNames, IM_ARRAYSIZE(actionTypeNames));

		} break;
		case ActionInputType::Mouse: 
		{
			ImGui::Combo("ActionType##NewActionInput", &newActionInputActionType, actionTypeNames, IM_ARRAYSIZE(actionTypeNames));
			ImGui::Combo("MouseButton##NewActionInput", &newActionInputButton, mouseButtonNames, IM_ARRAYSIZE(mouseButtonNames));
		} break;
		case ActionInputType::And:
		case ActionInputType::Or: 
		{
			const U32 inputCount = actionSystem.GetInputCount();
			if (inputCount < 2)
			{
				break;
			}
			std::vector<const char*> inputNames;
			inputNames.reserve(inputCount);
			for (U32 i = 0; i < inputCount; ++i)
			{
				inputNames.push_back(actionSystem.GetInputByIndex(i)->GetName().c_str());
			}

			ImGui::Combo("InputA##NewActionInput", &newActionInputAIndex, inputNames.data(), static_cast<int>(inputNames.size()));
			ImGui::Combo("InputB##NewActionInput", &newActionInputBIndex, inputNames.data(), static_cast<int>(inputNames.size()));
		} break;
		case ActionInputType::Not:
		{
			const U32 inputCount = actionSystem.GetInputCount();
			if (inputCount < 1)
			{
				break;
			}
			std::vector<const char*> inputNames;
			inputNames.reserve(inputCount);
			for (U32 i = 0; i < inputCount; ++i)
			{
				inputNames.push_back(actionSystem.GetInputByIndex(i)->GetName().c_str());
			}

			ImGui::Combo("Input##NewActionInput", &newActionInputAIndex, inputNames.data(), static_cast<int>(inputNames.size()));
		} break;
		default: assert(false); break;
		}

		bool validNewInput = true;

		const U32 nameLength = static_cast<U32>(strlen(newActionInputName));
		if (nameLength <= 0 || nameLength >= 255)
		{
			validNewInput = false;
		}
		if (actionInputType == ActionInputType::Variable || actionInputType == ActionInputType::Function || actionInputType == ActionInputType::Event)
		{
			validNewInput = false;
		}
		if (actionInputType == ActionInputType::Key)
		{
			validNewInput = false; // TODO : Remove when cool way to set the key
		}
		if (actionInputType == ActionInputType::And || actionInputType == ActionInputType::Or)
		{
			if (actionSystem.GetInputCount() < 2 || newActionInputAIndex == newActionInputBIndex)
			{
				validNewInput = false;
			}
		}
		if (actionInputType == ActionInputType::Not)
		{
			if (actionSystem.GetInputCount() < 1)
			{
				validNewInput = false;
			}
		}

		if (validNewInput)
		{
			if (ImGui::Button("Create##NewActionInput"))
			{
				switch (actionInputType)
				{
				case ActionInputType::Variable: assert(false); break;
				case ActionInputType::Function: assert(false); break;
				case ActionInputType::Event: assert(false); break;
				case ActionInputType::Key: actionSystem.AddInputKey(std::string(newActionInputName), static_cast<sf::Keyboard::Key>(newActionInputKey), static_cast<ActionType>(newActionInputActionType)); break;
				case ActionInputType::Mouse: actionSystem.AddInputMouse(std::string(newActionInputName), static_cast<sf::Mouse::Button>(newActionInputButton), static_cast<ActionType>(newActionInputActionType)); break;
				case ActionInputType::And:
				case ActionInputType::Or:
				{
					const U32 inputAID = actionSystem.GetInputByIndex(newActionInputAIndex)->GetID();
					const U32 inputBID = actionSystem.GetInputByIndex(newActionInputBIndex)->GetID();
					if (actionInputType == ActionInputType::And)
					{
						actionSystem.AddInputAnd(std::string(newActionInputName), inputAID, inputBID);
					}
					else
					{
						actionSystem.AddInputOr(std::string(newActionInputName), inputAID, inputBID);
					}
				} break;
				case ActionInputType::Not:
				{
					const U32 inputAID = actionSystem.GetInputByIndex(newActionInputAIndex)->GetID();
					actionSystem.AddInputNot(std::string(newActionInputName), inputAID);
				} break;
				default: assert(false); break;
				}

				
#ifdef ENLIVE_COMPILER_MSVC
				strcpy_s(newActionInputName, "");
#else
				strcpy(newActionInputName, "");
#endif // ENLIVE_COMPILER_MSVC
			}
		}
		else
		{
			ImGui::DisabledButton("Create##NewActionInput");
		}
	}

	ImGui::Separator();

	// Input list
	{
		ImGui::Text("Coming soon");
	}
}

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI