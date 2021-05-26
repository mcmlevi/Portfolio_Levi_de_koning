#include "pch.h"
#include "Core\EngineCore.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/PC/PCDevice.h"
#include "Platform/PC/GLFWInput.h"
#elif PLATFORM_SWITCH

#endif // PLATFORM_SWITCH

#include <Core/FrameListenerManager.h>
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
#ifdef PLATFORM_WINDOWS
#include "Tools/PC/PCTools.h"
#elif PLATFORM_SWITCH

#endif // PLATFORM_SWITCH
#endif // CONFIG_DEBUG || CONFIG_DEVELOP

namespace Frac
{
	void EngineCore::Initialize()
	{
		//Core programs.
		m_runState = true;
		m_memoryAllocator = std::make_unique<MemoryAllocator>();
		m_frameListenerManager = std::make_unique<Frac::FrameListenerManager>();
		m_resourceManager = std::make_unique<ResourceManager>();
		flecsWorld = std::make_unique<ECSWrapper>();

		// Render system creation, always pick one - initialization happens later.
		m_renderSystem = std::make_unique<RenderSystem>(*flecsWorld.get());
		m_prefabManager = std::make_unique<PrefabManager>(*m_renderSystem.get(), *flecsWorld.get());
		m_sceneManager = std::make_unique<SceneManager>(*m_renderSystem.get(), *m_prefabManager.get(), *flecsWorld.get());

		//Platform specific
#ifdef PLATFORM_WINDOWS
		Frac::Logger::GetInstance().Initialize("Log/FracLog.txt"); //Located in TDGame Files.
		m_device = std::make_unique<PCDevice>(*m_renderSystem.get());
		m_device->Initialize(1280, 768);
		m_inputSystem = std::make_unique<Frac::GLFWInput>(dynamic_cast<PCDevice*>(m_device.get())->GetWindow());
#elif PLATFORM_SWITCH

#endif // PLATFORM_SWITCH

		//m_renderSystem = std::make_unique<RenderSystem>("NVNRenderer"); Possible thing for later? :)
		
		//Platform specific rendering.
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
		m_DebugRenderer = std::make_unique<Frac::DebugRenderer>();
#ifdef PLATFORM_WINDOWS
		m_tools = std::make_unique<PCTools>();
#elif PLATFORM_SWITCH
		
#endif // PLATFORM_SWITCH
#endif // CONFIG_DEBUG || CONFIG_DEVELOP

		m_renderSystem->Initialize("OpenGLRenderer", m_device->GetWindowWidth(), m_device->GetWindowHeight());
		m_frameListenerManager->Initialize(*flecsWorld);
		m_transformSystemController = std::make_unique<TransformSystemController>(flecsWorld.get());
		m_transformSystemController->Initialize();
		
		m_textRenderer = std::make_unique<TextRenderSystem>(this, flecsWorld.get());

		m_audioManager = std::make_unique<AudioManager>(*flecsWorld.get());
	}

	void EngineCore::Run()
	{
		while (!m_device->ShouldClose() && m_runState)
		{
			m_device->PollEvents();
			m_device->SwapBuffers();
			m_inputSystem->Update(flecsWorld->getDeltaTime());
			flecsWorld->update();
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
			m_tools.get()->Update();
#endif // CONFIG_DEBUG || CONFIG_DEVELOP
			m_audioManager->Update(flecsWorld->getDeltaTime());
		}
	}

	MemoryAllocator& EngineCore::GetMemoryAllocator()
	{
		return *m_memoryAllocator;
	}

	IDevice& EngineCore::GetDevice()
	{
		return *m_device;
	}

	ResourceManager& Frac::EngineCore::GetResourceManager()
	{
		return *m_resourceManager;
	}

	Frac::InputSystem& EngineCore::GetInputSystem()
	{
		return *m_inputSystem;
	}

	Frac::TextRenderSystem& EngineCore::GetTextRenderSystem()
	{
		return *m_textRenderer;
	}

	FrameListenerManager& EngineCore::GetFrameListenerManager()
	{
		return *m_frameListenerManager;
	}

	Frac::ECSWrapper& EngineCore::GetWorld()
	{
		return *flecsWorld;
	}

	Frac::SceneManager& EngineCore::GetSceneManager() {
		return *m_sceneManager;
	}

	Frac::PrefabManager& EngineCore::GetPrefabManager()
	{
		return *m_prefabManager.get();
	}

	Frac::AudioManager& EngineCore::GetAudioManager()
	{
		return *m_audioManager;
	}

	void EngineCore::Shutdown()
	{
		m_renderSystem->Shutdown();
		Frac::Logger::GetInstance().Shutdown();
		m_runState = false;
	}

	void EngineCore::SetRunState(bool runState)
	{
		m_runState = runState;
	}

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	Frac::DebugRenderer& EngineCore::GetDebugRenderer()
	{
		return *m_DebugRenderer.get();
	}
#endif

} // namespace Frac