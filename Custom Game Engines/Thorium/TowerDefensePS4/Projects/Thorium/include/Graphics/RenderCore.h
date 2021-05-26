#pragma once
#include <vector>

#include <entt/single_include/entt/entt.hpp>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderContext.h>
#include <Graphics/Renderer.h>
#include <Graphics/DebugRenderer.h>
#include <Graphics/Window/Window.h>
#include <Graphics/ResourceLoader.h>

namespace Th
{
	/// <summary>
	/// The render core API is a cross platform instance, that manages the renderAPI of this engine.
	/// The sole purpose of the renderAPI is rendering, therefor it supports one method regarding that.
	/// </summary>
	class RenderCore
	{
	public:
		RenderCore() = default;
		~RenderCore() = default;

		void Initialize(Window& window);
		void Destroy();

		void Render(entt::registry& entities);

		ResourceLoader& GetResourceLoader();
		
		void SetActiveCamera(entt::entity camera);

	private:
		RenderDevice m_Device;
		Renderer m_Renderer;
		RenderContext m_RenderContext;
		ResourceLoader m_ResourceLoader;

#if defined _DEBUG
		DebugRenderer m_DebugRenderer;
#endif
	};
}