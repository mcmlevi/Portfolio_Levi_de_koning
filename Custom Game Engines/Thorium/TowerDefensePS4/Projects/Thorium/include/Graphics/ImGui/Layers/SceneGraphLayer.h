#pragma once
#include <Graphics/ImGui/Layers/IImguiLayer.h>
#include <entt/single_include/entt/entt.hpp>
#include <unordered_map>
#include <Components/TransformComponent.h>
namespace Th
{
	class NameComponent;

}

namespace Th
{

	
	class SceneGraphLayer : public IImguiLayer
	{
	public:
		SceneGraphLayer();
		virtual ~SceneGraphLayer() override;
		void Update() override;
	private:
		void AddPosData(Th::Components::TransformComponent& transform);
		void AddChild(entt::entity child);
		void AddGizmoWidget(Th::Components::TransformComponent& transform);
		void CameraSet(entt::entity& cam);
		
		struct Object
		{
			Object(Th::Components::TransformComponent* t, Th::NameComponent* n) :Transform{ t }, Name{ n }, Position{ t->GetPosition() }, Rotation{ t->GetEulerAngles() }, Scale{ t->GetScale() }{}
			Object() = default;
			Th::Components::TransformComponent* Transform;
			Th::NameComponent* Name;
			glm::vec3 Position{};
			glm::vec3 Rotation{};
			glm::vec3 Scale{};
		};
		
		Th::Components::TransformComponent* m_GizmoState{nullptr};
		entt::entity m_camera;
		bool m_active{ true };
	};
}


