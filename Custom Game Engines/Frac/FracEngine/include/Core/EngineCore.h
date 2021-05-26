#pragma once
#include "Core/MemoryAllocator.h"
#include "Platform/IDevice.h"
#include "Core/ResourceManager.h"
#include "Core/FrameListenerManager.h"
#include "InputSystem/InputSystem.h"
#include <flecs/flecs.h>
#include "Core/ECSWrapper.h"
#include "SceneManagement/System/TransformSystemController.h"
#include "FrameListener.h"
#include "Graphics/RenderSystem.h"
#include "Graphics/TextRenderSystem/TextRenderSystem.h"
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
#include "Graphics/DebugRenderer.h"
#include "Tools/ITools.h"
#endif
#include "Resources/PrefabManager.h"
#include "SceneManagement/SceneManager.h"
#include "Audio/AudioManager.h"

namespace Frac 
{
	class EngineConnector;
	class EngineCore 
	{
	public:
		/**
		* instantiates and Initializes all major systems
		*/
		void Initialize();

		/**
		* Core engine loop, updates all systems
		*/
		void Run();
		
		void Shutdown();

		void SetRunState(bool runState);
		
		MemoryAllocator& GetMemoryAllocator();
		IDevice& GetDevice();
		ResourceManager& GetResourceManager();
		Frac::InputSystem& GetInputSystem();
		Frac::TextRenderSystem& GetTextRenderSystem();
		Frac::FrameListenerManager& GetFrameListenerManager();
		Frac::ECSWrapper& GetWorld();
		Frac::SceneManager& GetSceneManager();
		Frac::PrefabManager& GetPrefabManager();
		Frac::AudioManager& GetAudioManager();

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
		Frac::DebugRenderer& GetDebugRenderer();
#endif

	private:
		static EngineCore& getInstance() 
		{
			static EngineCore instance;
			return instance;
		}

		EngineCore() = default;
		~EngineCore() = default;

		bool m_runState;

		std::unique_ptr<MemoryAllocator> m_memoryAllocator;
		std::unique_ptr<IDevice> m_device;
		std::unique_ptr<ResourceManager> m_resourceManager; //#TODO: Remove the resource manager, redundant class - we are using flecs for resource management.
		std::unique_ptr<Frac::InputSystem> m_inputSystem;
		std::unique_ptr<FrameListenerManager> m_frameListenerManager;
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
		std::unique_ptr<Frac::DebugRenderer> m_DebugRenderer;
		std::unique_ptr<ITools> m_tools;
#endif

		friend class EngineConnector;

		std::unique_ptr<PrefabManager> m_prefabManager;
		std::unique_ptr<ECSWrapper> flecsWorld;
		std::unique_ptr<RenderSystem> m_renderSystem;
		std::unique_ptr<SceneManager> m_sceneManager;
		std::unique_ptr<TextRenderSystem> m_textRenderer;
		std::unique_ptr<TransformSystemController> m_transformSystemController;
		std::unique_ptr<AudioManager> m_audioManager;
	};
} // namespace Frac