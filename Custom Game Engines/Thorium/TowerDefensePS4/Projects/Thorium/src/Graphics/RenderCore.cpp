#include <Thpch.h>
#include <Graphics/RenderCore.h>
#include <Events/EventMessenger.h>
namespace Th
{
	void RenderCore::Initialize(Window& window)
	{
		Th::EventMessenger::GetInstance().AddMessenger<entt::entity&>("ActiveCameraSet");
#if defined PLATFORM_WIN64
		m_Device.Initialize(false);
		m_RenderContext.Initialize(m_Device, window, 3);
		m_Renderer.Initialize(m_RenderContext);
		m_ResourceLoader.Initialize(m_Device);

#if defined _DEBUG
		m_DebugRenderer.Initialize(m_Device, window, 3);
#endif //debug

#elif PLATFORM_PS4
		m_Device.Initialize();
		m_ResourceLoader.Initialize();
		m_RenderContext.Initialize(m_Device, window);
		m_Renderer.Initialize(m_ResourceLoader, m_RenderContext);
#if defined _DEBUG
		m_DebugRenderer.Initialize();
#endif //debug
#endif //ps4
		
	}

	void RenderCore::Destroy()
	{
#if defined PLATFORM_WIN64
		m_Device.Flush();
		m_ResourceLoader.Destroy();

#if defined _DEBUG
		m_DebugRenderer.Destroy();
#endif //debug

		m_Renderer.Destroy();
		m_RenderContext.Destroy();
		m_Device.Destroy();

#elif PLATFORM_PS4
		m_Device.Destroy();
		m_ResourceLoader.Destroy();
		m_RenderContext.Destroy();
		m_Renderer.Destroy();

#if defined _DEBUG
		m_DebugRenderer.Destroy();
#endif //debug
#endif //ps4
	}

	void RenderCore::Render(entt::registry& entities)
	{
		m_Renderer.Render(entities);
	}

	ResourceLoader& RenderCore::GetResourceLoader()
	{
		return m_ResourceLoader;
	}

	void RenderCore::SetActiveCamera(entt::entity camera)
	{
		m_Renderer.SetActiveCamera(camera);
		Th::EventMessenger::GetInstance().EvokeMessenger("ActiveCameraSet",camera);
	}
}