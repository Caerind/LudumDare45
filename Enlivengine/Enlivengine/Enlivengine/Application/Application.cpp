#include <Enlivengine/Application/Application.hpp>

#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/System/String.hpp>

#ifdef ENLIVE_DEBUG
#include <Enlivengine/Graphics/DebugDraw.hpp>
#endif // ENLIVE_DEBUG

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Tools/ImGuiEntt.hpp>
#include <Enlivengine/Tools/ImGuiProfiler.hpp>
#include <Enlivengine/Tools/ImGuiDemoWindow.hpp>
#endif // ENLIVE_ENABLE_IMGUI

namespace en
{

Application::Application()
	: mStates(*this)
	, mWindow(sf::VideoMode(1024, 768), "")
	, mResourceManager()
	, mAudioSystem(mResourceManager)
	, mFps(0)
	, mRunning(true)
	, mTotalFrames(0)
	, mTotalDuration()
{
	LogManager::initialize();

	mWindowClosedSlot.connect(mWindow.onWindowClosed, [this](const en::Window*) { stop(); });

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().Initialize(mWindow);
	ImGuiProfiler::GetInstance().Register();
	ImGuiEntt::GetInstance().Register();
	ImGuiDemoWindow::GetInstance().Register();
#endif
}

Application::~Application()
{
	stop();

#ifdef ENLIVE_PLATFORM_MOBILE
	std::exit(0);
#endif
}

Window& Application::getWindow()
{
	return mWindow;
}

ResourceManager& Application::getResourceManager()
{
	return mResourceManager;
}

AudioSystem& Application::getAudio()
{
	return mAudioSystem;
}

void Application::stop()
{
	mAudioSystem.Stop();
	mAudioSystem.Clear();

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().Shutdown();
#endif

	if (mWindow.isOpen())
	{
		mWindow.close();
	}

	mRunning = false;
}

void Application::popState()
{
	mStates.popState();
}

void Application::clearStates()
{
	mStates.clearStates();
}

U32 Application::getFPS() const
{
	return mFps;
}


U32 Application::getTotalFrames() const
{
	return mTotalFrames;
}

Time Application::getTotalDuration() const
{
	return mTotalDuration.getElapsedTime();
}

void Application::run()
{
	const Time TimeUpdateFPS = seconds(0.5f);
	const Time TimePerFrame = seconds(1.0f / 60.0f);
	Time accumulator = Time::Zero;
	Time fpsAccumulator = Time::Zero;
	U32 framesFps = 0;
	Clock clock;

	// Running loop
	while (mRunning)
	{
#ifdef ENLIVE_ENABLE_PROFILE
		Profiler::GetInstance().StartFrame(mTotalFrames);
#endif

		// Time
		Time dt = clock.restart();

		// Usefull when using Breakpoints on Debugging
#ifdef ENLIVE_DEBUG
		if (dt > Time::Second)
		{
			dt = TimePerFrame;
		}
#endif

		accumulator += dt;
		fpsAccumulator += dt;

		events();

		// Fixed time 60 FPS
		while (accumulator >= TimePerFrame)
		{
			accumulator -= TimePerFrame;

#ifdef ENLIVE_ENABLE_IMGUI
			ImGuiToolManager::GetInstance().Update(mWindow, TimePerFrame);
#endif

			preUpdate();
			update(TimePerFrame);
			postUpdate();

#ifdef ENLIVE_ENABLE_IMGUI
			ImGuiToolManager::GetInstance().Render();
#endif
		}

		render();

		// FPS
		framesFps++;
		if (fpsAccumulator > TimeUpdateFPS)
		{
			mFps = static_cast<U32>(framesFps / TimeUpdateFPS.asSeconds());
			fpsAccumulator = Time::Zero;
			framesFps = 0;

#ifdef ENLIVE_DEBUG
			mWindow.setTitle("FPS : " + toString(mFps));
#endif
		}

		mTotalFrames++;

		// Stop ?
		if (!mWindow.isOpen() || mStates.getStateCount() == 0)
		{
			stop();
		}

#ifdef ENLIVE_ENABLE_PROFILE
		Profiler::GetInstance().EndFrame();
#endif
	}
}

void Application::events()
{
	ENLIVE_PROFILE_FUNCTION();

	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		// Might have used Signal of the Window, but clearer like this
		if (event.type == sf::Event::GainedFocus)
		{
			mAudioSystem.Play();
		}
		if (event.type == sf::Event::LostFocus)
		{
			mAudioSystem.Pause();
		}

#ifdef ENLIVE_ENABLE_IMGUI
		ImGuiToolManager::GetInstance().HandleEvent(event);
#endif	

		mStates.handleEvent(event);
	}
}

void Application::preUpdate()
{
	ENLIVE_PROFILE_FUNCTION();
	mAudioSystem.Update();
}

void Application::update(Time dt)
{
	ENLIVE_PROFILE_FUNCTION();
	mStates.update(dt);
}

void Application::postUpdate()
{
	ENLIVE_PROFILE_FUNCTION();
}

void Application::render()
{
	ENLIVE_PROFILE_FUNCTION();

	mWindow.clear();

	mStates.render(mWindow.getHandle());

#ifdef ENLIVE_DEBUG
	en::DebugDraw::render(mWindow.getHandle());
#endif // ENLIVE_DEBUG

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().RenderFrame(mWindow);
#endif // ENLIVE_ENABLE_IMGUI

	mWindow.display();
}

en::ScreenshotSystem& Application::getScreenshotSystem()
{
	return mScreenshotSystem;
}

} // namespace en
