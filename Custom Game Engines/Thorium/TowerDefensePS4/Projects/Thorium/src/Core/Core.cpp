#include <Thpch.h>

#include <Core/Core.h>
#include <Core/MessageLogger.h>
#include <Core/WorldManager.h>
#include <Events/InputHandler.h>
#include <Events/EventMessenger.h>
#include <Core/ResourceManager.h>

#include <Core/Audio/AudioSystem.h>
#include <Core/Audio/AudioComponent.h>
#include "Core/Audio/AudioManager.h"
#include <Utility/Timer.h>

namespace Th
{
	void Core::Initialize()
	{
		Th::WorldManager::GetSingleton().Initialize();
		ResourceManager::GetInstance().Initialize();

#if PLATFORM_WIN64
		ResourceManager::GetInstance().AddWildCard("[shaders]", "res/shaders");
		ResourceManager::GetInstance().AddWildCard("[models]", "res/models");
#elif PLATFORM_PS4
		ResourceManager::GetInstance().AddWildCard("[shaders]", "/app0/res/shaders");
		ResourceManager::GetInstance().AddWildCard("[models]", "/app0/res/models");
		ResourceManager::GetInstance().AddWildCard("[libs]", "/app0/lib");
#endif

		Th::MessageLogger::GetInstance().Initialize("Logs/ThoriumLog.txt");

#ifdef PLATFORM_WIN64
		m_Window.Initialize(1600, 900);
		EventMessenger::GetInstance().ConnectListener("OnWindowCloseRequest", &Core::ShutDown, this);
		Th::AudioManager::GetInstance().Initialize();
		m_audioSystem = new Th::AudioSystem{};
#elif PLATFORM_PS4
		m_Window.Initialize("PS4Window", 1920, 1080);
#endif
		m_RenderSystem.Initialize(m_Window);
	}

	void Core::MainLoop()
	{
		float dt{};
		while (m_Running)
		{
			Th::Timer timer{};
#if PLATFORM_WIN64 // Temporary fix, due to the usage of the wrongly used Fmod libraries.
			Th::AudioManager::GetInstance().Update(dt);
#endif
			Th::InputHandler::getInstance().UpdateInput();
			WorldManager::GetSingleton().Update(dt);
			m_RenderSystem.Render(WorldManager::GetSingleton().GetEntityRegister());
			m_Window.PollEvents();
			dt = timer.GetElapsedTime();
		}
	}

	void Core::ShutDown()
	{
#if PLATFORM_WIN64
		delete m_audioSystem;
		Th::AudioManager::GetInstance().ShutDown();
#endif

		EventMessenger::GetInstance().DisconnectListener("OnWindowCloseRequest", &Core::ShutDown, this);
		Th::InputHandler::getInstance().ShutDown();
		
		m_Running = false;
		m_RenderSystem.Destroy();
		m_Window.Destroy();

		ResourceManager::GetInstance().Destroy();
		MemoryManager::GetInstance().Shutdown();
		Th::MessageLogger::GetInstance().Shutdown();
	}
}
