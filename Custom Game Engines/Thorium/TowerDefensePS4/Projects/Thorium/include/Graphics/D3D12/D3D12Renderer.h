#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <glm/vec3.hpp>
#include <entt/single_include/entt/entt.hpp>
#include <Graphics/D3D12/D3D12RenderContext.h>
#include <Components/CameraComponent.h>

namespace Th
{
	/// <summary>
	/// The directX12 render system provides a fast way to get models on the screen using their correct properties.
	/// </summary>
	class D3D12Renderer
	{
	public:
		D3D12Renderer();
		~D3D12Renderer() = default;

		void Initialize(D3D12RenderContext& context);
		void Destroy();

		void SetActiveCamera(entt::entity entity);

		void Render(entt::registry& entities);

	private:
		D3D12RenderContext m_RenderContext;
		entt::entity m_ActiveCamera;
	};
}