#include "pch.h"
#include "CoreRenderAPI/Components/Transform.h"
#include "CoreRenderAPI/Components/Camera.h"
#include "CoreRenderAPI/Components/Light.h"
#include "CoreRenderAPI/Components/Material.h"
#include "Graphics/RenderSystem.h"
#include "CoreRenderAPI/RenderAPI.h"
#include "Core/ECSWrapper.h"

namespace Frac
{
	struct RenderUpdateTag{};
	RenderSystem::RenderSystem(ECSWrapper& wrapper) :
		m_entityManager(wrapper), 
		m_shadowRenderPassQuery(m_entityManager.makeQuery<TOR::Mesh, TOR::Material>("SHARED:TOR.Mesh,SHARED:TOR.Material")),
		m_sceneRenderPassQuery(m_entityManager.makeQuery<TOR::Mesh, TOR::Material>("SHARED:TOR.Mesh,SHARED:TOR.Material")),
		m_cameraQuery(m_entityManager.makeQuery<TOR::Camera>()),
		m_directionalLighShadowPassQuery(m_entityManager.makeQuery<TOR::DirectionalLight>()),
		m_directionalLightQuery(m_entityManager.makeQuery<TOR::DirectionalLight>()),
		m_pointLightQuery(m_entityManager.makeQuery<TOR::PointLight>()),
		m_spotLightQuery(m_entityManager.makeQuery<TOR::SpotLight>())
	{
	}

	void RenderSystem::Initialize(const std::string& dllName, unsigned viewportWidth, unsigned viewportHeight)
	{
		m_renderAPI = TOR::RenderAPI::CreateRenderAPI(dllName.c_str());
		m_renderAPI->Initialize(FileIO::GetPathFromWildcard("[Assets]"), viewportWidth, viewportHeight);

		auto e = m_entityManager.makeEntity("TransformSystemEntity");
		m_entityManager.add<RenderUpdateTag>(e);

		//Render system.
		std::function<void(EntityHandle, RenderUpdateTag&)> mainRenderSystem = [this](EntityHandle, RenderUpdateTag&)
		{
			this->SceneRenderPass();
		};

		//Rendering system.
		m_entityManager.makeSystem<RenderUpdateTag>(mainRenderSystem, Frac::ePipelineStage::postUpdate, "MainRenderSystem");
	}

	void RenderSystem::SceneRenderPass()
	{
		//Shadow render pass.
		std::function<void(EntityHandle, TOR::Mesh&, TOR::Material&)> shadowRenderPassFunc = [this](EntityHandle handle, TOR::Mesh& mesh, TOR::Material& material)
		{
			EntityHandle parent = m_entityManager.getParent(handle);
			const TOR::Transform* transform = m_entityManager.get<TOR::Transform>(parent);

			if (transform != nullptr)
			{
				//Bind the mesh and prepare the mesh for recording.
				m_renderAPI->BindMeshForRecording(mesh.Id);
				m_renderAPI->SetMeshShadowProperties(mesh.Id, transform->ModelTransformData);

				//Render the model and unbind it if applicable.
				m_renderAPI->Render(mesh.Id, mesh.IndiceCount);
#if CONFIG_DEBUG
				m_renderAPI->UnbindMesh(mesh.Id);
#endif
			}
		};

		std::function<void(EntityHandle, TOR::DirectionalLight&)> directionalLighShadowPasstQueryFunc = [this, &shadowRenderPassFunc](EntityHandle handle, TOR::DirectionalLight& light)
		{
			const TOR::Transform& lightTransform = *m_entityManager.get<TOR::Transform>(handle);
			m_renderAPI->SetupShadowPass(glm::vec3(35.0f, 90.0f, 100.0f), glm::vec3(lightTransform.ModelTransformData[2]));
			m_shadowRenderPassQuery.Each(shadowRenderPassFunc);
		};
		m_directionalLightQuery.Each(directionalLighShadowPasstQueryFunc);
		m_renderAPI->EndShadowPass();

		//Scene render pass.
		m_renderAPI->SetupScenePass();
		std::function<void(EntityHandle, TOR::Mesh&, TOR::Material&)> sceneRenderPassFunc = [this](EntityHandle handle, TOR::Mesh& mesh, TOR::Material& material)
		{
			EntityHandle parent = m_entityManager.getParent(handle);
			const TOR::Transform* transform = m_entityManager.get<TOR::Transform>(parent);

			if (transform != nullptr)
			{
				//Bind the mesh and prepare the mesh for recording.
				m_renderAPI->BindMeshForRecording(mesh.Id);
				m_renderAPI->SetMeshProperties(mesh.Id, transform->ModelTransformData);

				TOR::PBRMaterial pbrMaterial
				{
					material,
					m_entityManager.get<TOR::AlbedoTexture>(handle),
					m_entityManager.get<TOR::NormalTexture>(handle),
					m_entityManager.get<TOR::MetalRougnessAOTexture>(handle),
					m_entityManager.get<TOR::EmissionTexture>(handle)
				};

				//Set the material properties of the mesh.
				m_renderAPI->SetMaterialProperties(mesh.Id, pbrMaterial);

				//Set the active camera properties for rendering this mesh.
				UpdateCamera(mesh.Id);
				UpdateLights(mesh.Id, material);

				//Render the model and unbind it if applicable.
				m_renderAPI->Render(mesh.Id, mesh.IndiceCount);
	#if CONFIG_DEBUG
				m_renderAPI->UnbindMesh(mesh.Id);
	#endif
			}
		};
		m_sceneRenderPassQuery.Each(sceneRenderPassFunc);
		m_renderAPI->EndScenePass();
	}

	void RenderSystem::UpdateCamera(unsigned meshId)
	{
		std::function<void(EntityHandle, TOR::Camera&)> cameraQueryFunc = [this, meshId](EntityHandle handle, TOR::Camera& camera)
		{
			if (camera.IsActive)
			{
				const TOR::Transform& transformOfCamera = *m_entityManager.get<TOR::Transform>(handle);
				m_renderAPI->SetCameraProperties(meshId, transformOfCamera.ModelTransformData, camera);
			}
		};
		m_cameraQuery.Each(cameraQueryFunc);
	}

	void RenderSystem::UpdateLights(unsigned meshId, TOR::Material& material)
	{
		//DirectionalLights.
		std::function<void(EntityHandle, TOR::DirectionalLight&)> directionalLightQueryFunc = [this, meshId, &material](EntityHandle handle, TOR::DirectionalLight& light)
		{
			const TOR::Transform& lightTransform = *m_entityManager.get<TOR::Transform>(handle);
			m_renderAPI->SetDirectionalLightProperties(meshId, lightTransform.ModelTransformData, light, material);
		};
		m_directionalLightQuery.Each(directionalLightQueryFunc);

		//Pointlights.
		std::function<void(EntityHandle, TOR::PointLight&)> pointlightQueryFunc = [this, meshId, &material](EntityHandle handle, TOR::PointLight& light)
		{
			const TOR::Transform& lightTransform = *m_entityManager.get<TOR::Transform>(handle);
			m_renderAPI->SetPointlightProperties(meshId, lightTransform.ModelTransformData, light, material);
		};
		m_pointLightQuery.Each(pointlightQueryFunc);

		//Spotlights.
		std::function<void(EntityHandle, TOR::SpotLight&)> spotlightQueryFunc = [this, meshId, &material](EntityHandle handle, TOR::SpotLight& light)
		{
			const TOR::Transform& lightTransform = *m_entityManager.get<TOR::Transform>(handle);
			m_renderAPI->SetSpotlightProperties(meshId, lightTransform.ModelTransformData, light, material);
		};
		m_spotLightQuery.Each(spotlightQueryFunc);
	}

	void RenderSystem::ResizeViewport(unsigned viewportWidth, unsigned viewportHeight)
	{
		m_renderAPI->ResizeViewport(viewportWidth, viewportHeight);
	}

	void RenderSystem::Shutdown()
	{
		m_renderAPI->Shutdown();
	}

	TOR::RenderAPI& RenderSystem::GetRenderAPI() const
	{
		return *m_renderAPI;
	}
}