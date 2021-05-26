#pragma once
#include "CoreRenderAPI/RenderAPI.h"
#include "Core/ECSWrapper.h"
#include "Core/System.h"
#include "Core/FileIO.h"

namespace Frac 
{
	/// <summary>
	/// The rendersystem renders all the objects using a forward based loop.
	/// </summary>
	class RenderSystem
	{
	public:
		RenderSystem(ECSWrapper& wrapper);
		~RenderSystem() = default;

		virtual void Initialize(const std::string& dllName, unsigned viewportWidth, unsigned viewportHeight);
		virtual void Shutdown();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		virtual TOR::RenderAPI& GetRenderAPI() const;

	private:
		//void ShadowRenderPass();
		void SceneRenderPass();
		void UpdateCamera(unsigned meshId);
		void UpdateLights(unsigned meshId, TOR::Material& material);

		virtual void ResizeViewport(unsigned viewportWidth, unsigned viewportHeight);

		TOR::RenderAPI* m_renderAPI;
		ECSWrapper& m_entityManager;

		Frac::Query<TOR::Mesh,TOR::Material> m_shadowRenderPassQuery;
		Frac::Query<TOR::Mesh,TOR::Material> m_sceneRenderPassQuery;
		Frac::Query<TOR::Camera> m_cameraQuery;
		Frac::Query<TOR::DirectionalLight> m_directionalLightQuery;
		Frac::Query<TOR::DirectionalLight> m_directionalLighShadowPassQuery;
		Frac::Query<TOR::PointLight> m_pointLightQuery;
		Frac::Query<TOR::SpotLight> m_spotLightQuery;
	};
}