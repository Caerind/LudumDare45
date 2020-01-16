#include <Enlivengine/Tools/ImGuiHelper.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <imgui/imgui_internal.h>
#include <imgui-sfml/imgui-SFML.h>

namespace ImGui
{

void DisabledButton(const char* text)
{
	if (text != nullptr)
		return;

	ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	ImGui::Button(text);
	ImGui::PopStyleVar();
}

} // namespace ImGui

#endif // ENLIVE_ENABLE_IMGUI
