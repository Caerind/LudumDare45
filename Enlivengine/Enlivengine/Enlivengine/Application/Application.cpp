#include <Enlivengine/Application/Application.hpp>

#include <Enlivengine/System/Config.hpp>
#include <Enlivengine/System/String.hpp>

#ifdef ENLIVE_DEBUG
#include <Enlivengine/Graphics/DebugDraw.hpp>
#endif // ENLIVE_DEBUG

#ifdef ENLIVE_ENABLE_IMGUI
#include <Enlivengine/Application/ImGuiToolManager.hpp>
#include <Enlivengine/Tools/ImGuiConsole.hpp>
#include <Enlivengine/Tools/ImGuiEntt.hpp>
#include <Enlivengine/Tools/ImGuiLogger.hpp>
#include <Enlivengine/Tools/ImGuiProfiler.hpp>
#include <Enlivengine/Tools/ImGuiDemoWindow.hpp>
#include <Enlivengine/Tools/ImGuiResourceBrowser.hpp>
#endif // ENLIVE_ENABLE_IMGUI

namespace en
{

Application::Application()
	: mStates(*this)
	, mWindow(sf::VideoMode(1024, 768), "")
	, mFps(0)
	, mRunning(true)
	, mTotalFrames(0)
	, mTotalDuration()
{
#ifdef ENLIVE_ENABLE_LOG
	LogManager::GetInstance().Initialize();
#endif // ENLIVE_ENABLE_LOG

	ImGuiConsole::GetInstance().RegisterConsole();

	mWindowClosedSlot.connect(mWindow.onWindowClosed, [this](const en::Window*) { Stop(); });

	RegisterTools();

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiResourceBrowser::GetInstance().LoadResourceInfosFromFile(PathManager::GetInstance().GetAssetsPath() + "resources.xml");
#endif // ENLIVE_ENABLE_IMGUI
}

Application::~Application()
{
	Stop();
}

Window& Application::GetWindow()
{
	return mWindow;
}

void Application::Stop()
{
	AudioSystem::GetInstance().Stop();
	AudioSystem::GetInstance().Clear();

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiResourceBrowser::GetInstance().SaveResourceInfosToFile(PathManager::GetInstance().GetAssetsPath() + "resources.xml");
#endif // ENLIVE_ENABLE_IMGUI

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().Shutdown();
#endif // ENLIVE_ENABLE_IMGUI

	if (mWindow.isOpen())
	{
		mWindow.close();
	}

	mRunning = false;

	std::exit(EXIT_SUCCESS);
}

void Application::PopState()
{
	mStates.popState();
}

void Application::ClearStates()
{
	mStates.clearStates();
}

U32 Application::GetFPS() const
{
	return mFps;
}

U32 Application::GetTotalFrames() const
{
	return mTotalFrames;
}

Time Application::GetTotalDuration() const
{
	return mTotalDuration.getElapsedTime();
}

void Application::Run()
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
#endif // ENLIVE_ENABLE_PROFILE

		{
			ENLIVE_PROFILE_SCOPE(MainFrame);

			// Time
			Time dt = clock.restart();

			// Usefull when using Breakpoints on Debugging
#ifdef ENLIVE_DEBUG
			if (dt > Time::Second)
			{
				dt = TimePerFrame;
			}
#endif // ENLIVE_DEBUG

			accumulator += dt;
			fpsAccumulator += dt;

			Events();

			// Fixed time 60 FPS
			while (accumulator >= TimePerFrame)
			{
				accumulator -= TimePerFrame;

#ifdef ENLIVE_ENABLE_IMGUI
				ImGuiToolManager::GetInstance().Update(mWindow, TimePerFrame);
#endif // ENLIVE_ENABLE_IMGUI

				PreUpdate();
				Update(TimePerFrame);
				PostUpdate();

#ifdef ENLIVE_ENABLE_IMGUI
				ImGuiToolManager::GetInstance().Render();
#endif // ENLIVE_ENABLE_IMGUI
			}

			Render();

			// FPS
			framesFps++;
			if (fpsAccumulator > TimeUpdateFPS)
			{
				mFps = static_cast<U32>(framesFps / TimeUpdateFPS.asSeconds());
				fpsAccumulator = Time::Zero;
				framesFps = 0;

#ifdef ENLIVE_DEBUG
				mWindow.setTitle("FPS : " + toString(mFps));
#endif // ENLIVE_DEBUG
			}

			mTotalFrames++;

			// Stop ?
			if (!mWindow.isOpen() || mStates.getStateCount() == 0)
			{
				Stop();
			}
		}

#ifdef ENLIVE_ENABLE_PROFILE
		Profiler::GetInstance().EndFrame();
#endif // ENLIVE_ENABLE_PROFILE
	}
}

void Application::Events()
{
	ENLIVE_PROFILE_FUNCTION();

	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		// Might have used Signal of the Window, but clearer like this
		if (event.type == sf::Event::GainedFocus)
		{
			AudioSystem::GetInstance().Play();
		}
		if (event.type == sf::Event::LostFocus)
		{
			AudioSystem::GetInstance().Pause();
		}

#ifdef ENLIVE_ENABLE_IMGUI
		ImGuiToolManager::GetInstance().HandleEvent(event);
		if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P && event.key.control == true)
		{
#ifdef ENLIVE_ENABLE_PROFILE
			if (ImGuiProfiler::GetInstance().CanCurrentFrameBeCaptured())
			{
				ImGuiProfiler::GetInstance().CaptureCurrentFrameAndOpenProfiler();
			}
			else
			{
				ImGuiProfiler::GetInstance().SetEnabled(true);
			}
#endif // ENLIVE_ENABLE_PROFILE
		}
#endif	

		mStates.handleEvent(event);
	}
}

void Application::PreUpdate()
{
	ENLIVE_PROFILE_FUNCTION();
	AudioSystem::GetInstance().Update();
}

void Application::Update(Time dt)
{
	ENLIVE_PROFILE_FUNCTION();
	mStates.update(dt);
}

void Application::PostUpdate()
{
	ENLIVE_PROFILE_FUNCTION();
}

void Application::Render()
{
	ENLIVE_PROFILE_FUNCTION();

	mWindow.clear();

	mStates.render(mWindow.getHandle());

#ifdef ENLIVE_DEBUG
	DebugDraw::render(mWindow.getHandle());
#endif // ENLIVE_DEBUG

#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().RenderFrame(mWindow);
#endif // ENLIVE_ENABLE_IMGUI

	mWindow.display();
}

void Application::RegisterTools()
{
#ifdef ENLIVE_ENABLE_IMGUI
	ImGuiToolManager::GetInstance().Initialize(mWindow);

	// Main
#ifdef ENLIVE_ENABLE_LOG
	ImGuiLogger::GetInstance().Register();
#endif // ENLIVE_ENABLE_LOG
	ImGuiResourceBrowser::GetInstance().Register();
	ImGuiConsole::GetInstance().Register();
	ImGuiDemoWindow::GetInstance().Register();

	// Engine
	ImGuiEntt::GetInstance().Register();

	// Game
#ifdef ENLIVE_ENABLE_PROFILE
	ImGuiProfiler::GetInstance().Register();
#endif // ENLIVE_ENABLE_PROFILE
#endif // ENLIVE_ENABLE_IMGUI
}

ScreenshotSystem& Application::GetScreenshotSystem()
{
	return mScreenshotSystem;
}

} // namespace en
