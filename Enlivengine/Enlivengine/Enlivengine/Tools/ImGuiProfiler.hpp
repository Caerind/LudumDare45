#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>

#ifdef ENLIVE_ENABLE_IMGUI

#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/System/Profiler.hpp>

namespace en
{

class ImGuiProfiler : public ImGuiTool
{
public:
	static ImGuiProfiler& GetInstance();

	virtual ImGuiToolTab GetTab() const;
	virtual const char* GetName() const;

	virtual void Display();

private:
	ImGuiProfiler();
};

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
