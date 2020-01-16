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

	virtual int GetWindowFlags() const;

	virtual void Display();

	// Shortcuts for Profiler
	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	bool CanCurrentFrameBeCaptured() const;
	void CaptureCurrentFrame();
	void CaptureFrames(U32 nbFrames);
	bool IsCapturing() const;

	// Specific methods
	void CaptureCurrentFrameAndOpenProfiler();

private:
	void DisplayFrame(const ProfilerFrame& frame) const;
	void ForceResize();

private:
	U32 mCaptureFrames;
	U32 mCurrentFrameIndex;
	bool mForceResize;

private:
	ImGuiProfiler();
};

} // namespace en

#endif // ENLIVE_ENABLE_IMGUI
