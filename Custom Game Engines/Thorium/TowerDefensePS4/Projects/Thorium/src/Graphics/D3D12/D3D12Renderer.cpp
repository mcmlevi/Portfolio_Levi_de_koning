#include <Thpch.h>
#include <Graphics/D3D12/D3D12Renderer.h>
#include <Utility/D3DErrorHandler.h>
#include <Events/EventMessenger.h>
#include <Graphics/D3D12/D3D12CommandQueue.h>
#include <Graphics/D3D12/D3D12CommandList.h>
#include <Resources/D3D12/D3D12Mesh.h>
#include <Components/TransformComponent.h>
#include <Components/ModelComponent.h>
#include <Core/MessageLogger.h>

namespace Th
{
	D3D12Renderer::D3D12Renderer() : m_ActiveCamera(entt::null) {}

	void D3D12Renderer::Initialize(D3D12RenderContext& context)
	{
		EventMessenger::GetInstance().AddMessenger<ComPtr<ID3D12GraphicsCommandList2>>("PreDebugRender");
		EventMessenger::GetInstance().AddMessenger<ComPtr<ID3D12GraphicsCommandList2>>("PostDebugRender");

		m_RenderContext = context;
	}

	void D3D12Renderer::Destroy()
	{
		EventMessenger::GetInstance().RemoveMessenger("PostDebugRender");
		EventMessenger::GetInstance().RemoveMessenger("PreDebugRender");
	}

	void D3D12Renderer::SetActiveCamera(entt::entity entity)
	{
		m_ActiveCamera = entity;
	}

	void D3D12Renderer::Render(entt::registry& entities)
	{
		D3D12CommandList* commandList = m_RenderContext.ClearColor();
		
#if defined _DEBUG	
		if (m_ActiveCamera == entt::null)
		{
			LOGERROR("No camera has been set in the renderCore! Make sure to assign one.");
			return;
		}

		EventMessenger::GetInstance().EvokeMessenger("PreDebugRender", commandList->GetGraphicsCommandList());
#endif

		auto view = entities.view<Components::TransformComponent, Components::CameraComponent>();
		Components::TransformComponent& cameraTransform = view.get<Components::TransformComponent>(m_ActiveCamera);
		Components::CameraComponent& cameraComponent = view.get<Components::CameraComponent>(m_ActiveCamera);

		glm::mat4 projMat = cameraComponent.GetProjection();
		glm::mat4 viewMat = glm::inverse(cameraTransform.GetWorldMatrix());

		entities.view<Components::TransformComponent, Components::ModelComponent>().each([commandList, projMat, viewMat, this](auto& transform, auto& model)
		{
			for (int i = 0; i < model.GetMeshCount(); i++)
			{
				model[i].RenderShader.Bind(commandList);
				model[i].RenderShader.SetRootParams(commandList, projMat * viewMat * transform.GetWorldMatrix());
				model[i].RenderMesh.RecordDrawCommand(commandList, 1, 0);
			}
		});
		
#if defined _DEBUG
		EventMessenger::GetInstance().EvokeMessenger("PostDebugRender", commandList->GetGraphicsCommandList());
#endif
		m_RenderContext.SwapBuffers(commandList);
	}
}