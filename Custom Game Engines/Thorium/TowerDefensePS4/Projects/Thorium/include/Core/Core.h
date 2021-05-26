#pragma once

#include <Graphics/Window/Window.h>
#include <Graphics/RenderCore.h>

namespace Th 
{
	class AudioSystem;
	/// <summary>
	/// Represents the core of the engine, should be created on startup.
	/// </summary>
	class Core 
	{
	public:
		Core() = default;
		virtual ~Core() = default;

	public:
		virtual void Initialize();
		virtual void ShutDown();

	protected:
		void MainLoop();
		RenderCore m_RenderSystem;
	
	private:
		Window m_Window;
		bool m_Running = true;

#if PLATFORM_WIN64
		Th::AudioSystem* m_audioSystem;
#endif
	};

	Core* CreateGame();
}

