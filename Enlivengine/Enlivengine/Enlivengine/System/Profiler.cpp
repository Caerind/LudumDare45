#include <Enlivengine/System/Profiler.hpp>

#ifdef ENLIVE_ENABLE_PROFILE

#include <Enlivengine/System/Assert.hpp>

namespace en
{

Time ProfilerTask::GetDuration() const
{ 
	return end - start;
}

Time ProfilerFrame::GetDuration() const
{
	return end - start;
}

F32 ProfilerFrame::GetPercent(const Time& subDuration) const
{
	return (100.f * subDuration.asMicroseconds()) / (1.0f * GetDuration().asMicroseconds());
}

U32 ProfilerFrame::GetMaxDepth() const
{
	U32 maxDepth = 0;
	const size_t size = tasks.size();
	for (size_t i = 0; i < size; ++i)
	{
		if (tasks[i].depth > maxDepth)
		{
			maxDepth = tasks[i].depth;
		}
	}
	return maxDepth;
}

Profile::Profile(const char* functionName)
{
	Profiler::GetInstance().StartFunction(functionName);
}

Profile::~Profile()
{
	Profiler::GetInstance().EndFunction();
}

Profiler& Profiler::GetInstance()
{
	static Profiler instance;
	return instance;
}

void Profiler::SetFrameCapacity(U32 capacity)
{
	mProfilerFrames.reserve(capacity);
}

U32 Profiler::GetFrameCapacity() const
{
	return static_cast<U32>(mProfilerFrames.capacity());
}

void Profiler::CaptureFrames(U32 nbFrames)
{
	if (nbFrames > 0)
	{
		mCapturing = true;
		mCapturingFrames = nbFrames;
		mProfilerFrames.clear();
		mProfilerFrames.resize(nbFrames);
	}
}

bool Profiler::IsCapturing() const
{
	return mCapturing;
}

const std::vector<ProfilerFrame>& Profiler::GetProfilerFrames() const
{
	assert(!IsCapturing());
	return mProfilerFrames;
}

void Profiler::StartFrame(U32 frameNumber)
{
	mDepthCounter = 0;
	mCurrentFrame.frame = frameNumber;
	mCurrentFrame.start = Time::now();
	mCurrentFrame.end = mCurrentFrame.start;
	mCurrentFrame.tasks.clear();
	mCurrentFrame.tasks.reserve(kProfilesPerFrameCapacity);
}

void Profiler::EndFrame()
{
	assert(mDepthCounter == 0);
	if (IsCapturing())
	{
		mCurrentFrame.end = Time::now();
		const U32 index = static_cast<U32>(mProfilerFrames.size()) - mCapturingFrames;
		mProfilerFrames[index] = std::move(mCurrentFrame);
		mCapturingFrames--;
		if (mCapturingFrames == 0)
		{
			mCapturing = false;
		}
	}
}

void Profiler::StartFunction(const char* name)
{
	ProfilerTask task;
	task.name = name;
	task.start = Time::now();
	task.depth = mDepthCounter++;
	mCurrentFrame.tasks.push_back(task);
}

void Profiler::EndFunction()
{
	mCurrentFrame.tasks.back().end = Time::now();
	mDepthCounter--;
}

Profiler::Profiler()
	: mCapturing(false)
	, mCapturingFrames(0)
	, mCurrentFrame()
	, mProfilerFrames()
{
	SetFrameCapacity(kDefaultFramesCapacity);
	mCurrentFrame.tasks.reserve(kProfilesPerFrameCapacity);
}

/*
void ConsoleProfiler::DisplayFrame(const ProfilerFrame& frame)
{
	ENLIVE_PROFILE_FUNCTION();

	// TODO : Move this elsewhere ?
    #ifdef ENLIVE_COMPILER_MSVC
        system("cls");
	#else
        printf("\e[1;1H\e[2J"); // Should work on ANSI
	#endif

	printf("Frame %u, duration : %I64dus\n", frame.frame, frame.GetDuration().asMicroseconds());
	for (const auto& task : frame.tasks)
	{
		const U32 depth = task.depth;
		for (U32 i = 0; i < depth; ++i)
		{
			printf("  ");
		}
		printf("-%s, duration : %I64dus, percent : %f\n", task.name, task.GetDuration().asMicroseconds(), frame.GetPercent(task.GetDuration()));
	}
}
*/

} // namespace en

#endif // ENLIVE_ENABLE_PROFILE
