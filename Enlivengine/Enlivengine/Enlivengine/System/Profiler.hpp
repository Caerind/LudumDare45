#pragma once

#include <Enlivengine/System/PrimitiveTypes.hpp>

#ifdef ENLIVE_ENABLE_PROFILE

#include <Enlivengine/System/CompilerTraits.hpp>
#include <Enlivengine/System/Time.hpp>

#include <vector>

namespace en
{

struct ProfilerTask
{
	const char* name;
	Time start;
	Time end;
	U32 depth;
	// TODO : Color color;

	Time GetDuration() const;
};

struct ProfilerFrame
{
	U32 frame;
	Time start;
	Time end;
	std::vector<ProfilerTask> tasks;

	Time GetDuration() const;
	F32 GetPercent(const Time& subDuration) const;
	U32 GetMaxDepth() const;
};

class Profile
{
public:
	Profile(const char* functionName);
	~Profile();
};

class Profiler
{
public:
	static Profiler& GetInstance();

	void SetFrameCapacity(U32 capacity);
	U32 GetFrameCapacity() const;

	void CaptureFrames(U32 nbFrames);
	bool IsCapturing() const;

	const std::vector<ProfilerFrame>& GetProfilerFrames() const;

private:
	friend class Application;
	void StartFrame(U32 frameNumber);
	void EndFrame();

	friend class Profile;
	void StartFunction(const char* name);
	void EndFunction();

	static constexpr U32 kDefaultFramesCapacity{ 10 };
	static constexpr U32 kProfilesPerFrameCapacity{ 256 };
	Profiler();

private:
	// Working data
	bool mCapturing;
	U32 mCapturingFrames;
	U32 mDepthCounter;
	ProfilerFrame mCurrentFrame;

	// Stored data
	std::vector<ProfilerFrame> mProfilerFrames;
};

#define ENLIVE_PROFILE_FUNCTION() en::Profile functionProfile(ENLIVE_FUNCTION);
#define ENLIVE_PROFILE_SCOPE(name) en::Profile scope##name(#name);

} // namespace en

#else

#define ENLIVE_PROFILE_FUNCTION()
#define ENLIVE_PROFILE_SCOPE(name)

#endif // ENLIVE_ENABLE_PROFILE