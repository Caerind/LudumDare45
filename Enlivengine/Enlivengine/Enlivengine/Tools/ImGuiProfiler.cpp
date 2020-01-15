#include <Enlivengine/Tools/ImGuiProfiler.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <imgui/imgui.h>

namespace en
{

ImGuiProfiler& ImGuiProfiler::GetInstance()
{
	static ImGuiProfiler instance;
	return instance;
}

ImGuiToolTab ImGuiProfiler::GetTab() const
{
	return ImGuiToolTab::Game;
}

const char* ImGuiProfiler::GetName() const
{
	return ICON_FA_CLOCK " Profiler";
}

void ImGuiProfiler::Display()
{
	if (ImGui::Button("Test"))
	{

	}
}

ImGuiProfiler::ImGuiProfiler()
	: ImGuiTool()
{
}

/*
void ImGuiProfiler::draw()
{
	ENLIVE_PROFILE_FUNCTION();

	if (!ImGui::Begin("Profiler"))
	{
		ImGui::End();
		return;
	}

	if (!mPaused && ImGui::Button("Pause"))
	{
		mPaused = true;
	}
	else if (mPaused && ImGui::Button("Play"))
	{
		mPaused = false;
	}
	ImGui::SameLine();
	if (!mImportant && ImGui::Button("Important frames"))
	{
		mImportant = true;
	}
	else if (mImportant && ImGui::Button("All frames"))
	{
		mImportant = false;
	}

	ImGui::Text("Frame %d, duration : %d us", mFrame.getFrameNumber(), mFrame.getDuration().asMicroseconds());

	ImGui::BeginChild("Scrolling", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

	const std::vector<ProfilerFunctionCall>& calls(mFrame.getCalls());
	for (U32 i = 0; i < calls.size(); i++)
	{
		if (calls[i].getLevel() == 0)
		{
			drawFunctionCall(calls[i]);
		}
	}

	ImGui::PopStyleVar();
	ImGui::EndChild();

	ImGui::End();
}

void ImGuiProfiler::drawFunctionCall(const ProfilerFunctionCall& fc)
{
	const std::vector<ProfilerFunctionCall>& calls(mFrame.getCalls());

	const std::string str = std::string(fc.getName()) + ", duration : " + std::to_string(fc.getDuration().asMicroseconds()) + "us, percent : " + std::to_string(fc.getPercent(mFrame.getDuration()));

	if (ImGui::TreeNode(str.c_str()))
	{
		if (fc.hasChildren())
		{
			for (U32 i = 0; i < calls.size(); i++)
			{
				if (calls[i].getParent() == fc.getId())
				{
					drawFunctionCall(calls[i]);
				}
			}
		}
		ImGui::TreePop();
	}
}
*/

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
