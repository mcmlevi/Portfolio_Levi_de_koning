#pragma once
#include <vector>
#include <Core/Systems.h>
#include <string>
namespace Th
{

	class WorldManager
	{
	
	private:

		entt::registry m_Register;
		std::vector< Systems* > m_Systems;

	public:

		WorldManager(const WorldManager&) = delete;
		~WorldManager();
		void Initialize();
		/// <summary>
		/// </summary>
		/// <returns>Returns the instance of the worldmanager.</returns>
		static WorldManager& GetSingleton();

		/// <summary>
		/// Updates all the registered systems.
		/// </summary>
		void Update(float);

		/// <summary>
		/// Add a system to the register.
		/// </summary>
		/// <param name="System*"></param>
		void RegisterSystem(Systems*);
		 
		/// <summary>
		/// </summary>
		/// <returns>entity register from the worldmanager containing all information about the components and entities in the world. 
		/// Entities and components can be fetched, created or removed from here.</returns>
		entt::registry& GetEntityRegister();

		entt::entity CreateEntity(const std::string& name, TransformComponent& transform);

		/// <summary>
		/// Delete an entity from the entt register.
		/// </summary>
		void DeleteEntity(entt::entity a_Entity);

		/// <summary>
		/// Remove component of type from the entity if it can be found on it.
		/// </summary>
		template<typename  T>
		void RemoveComponent(entt::entity a_Entity);

		/// <summary>
		/// Remove all components of an entity.
		/// </summary>
		void RemoveAllComponents(entt::entity a_Entity);
		entt::entity GetWorldRoot();
		template<typename Component>
		entt::entity RayCast(glm::vec2 screenPos, entt::entity Camera);
	private:
		glm::vec3 Unproject(glm::vec3& v, entt::entity Camera);
	private:
		entt::entity m_WorldRoot = entt::null;
		WorldManager();
	};

	template<typename  T>
	void WorldManager::RemoveComponent(entt::entity a_Entity)
	{
		m_Register.remove_if_exists<T>(a_Entity);
	}

	template <typename Component >
	entt::entity WorldManager::RayCast(glm::vec2 screenPos, entt::entity Camera)
	{
		Th::Components::TransformComponent& CamTransform = m_Register.get<Th::TransformComponent>(Camera);
		float Px = (2.f * ((screenPos.x + 0.5) / 1600.f) - 1.f);
		float Py = (1.f - 2.f * ((screenPos.y + 0.5) / 900.f));
		
		// Unproject a vector at the far plane
		glm::vec3 toVec{ glm::vec3(Px, Py, 1.0f) };
		const glm::vec3 to = Unproject(toVec, Camera);

		// Make a ray out of the two vectors
		
		glm::vec3 origin = CamTransform.GetPosition();
		glm::vec3 dir = glm::normalize(to);		
		//glm::vec3 dir = glm::normalize(wpos);
		//glm::vec3 origin = CamTransform.GetPosition();
		auto view = m_Register.view<Th::ModelComponent, Th::TransformComponent, Component>();
		float distance = FLT_MAX; 
		entt::entity foundEntity = entt::null;
		for (auto entity : view)
		{
				Th::ModelComponent& model = view.template get<Th::ModelComponent>(entity);
				Th::TransformComponent& transform = view.template get<Th::TransformComponent>(entity);
				
				glm::vec3 hit{};
				if(model.getCollider().intersect(origin, dir,transform.GetWorldMatrix(), hit))
				{
					if(glm::length2(origin -hit) < distance)
					{
						distance = glm::length2(origin - hit);
						foundEntity = entity;
					}
				}
		}


		return foundEntity;
	}
}


