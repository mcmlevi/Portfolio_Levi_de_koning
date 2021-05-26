#pragma once

// Only disable these warning messages when checking in EnTT.
#ifdef PLATFORM_WIN64
#pragma warning (push)
#pragma warning (disable : 26495 26439)
#elif PLATFORM_PS4
#pragma GCC diagnostic push
//#pragma GCC diagnostic ignore
#endif

#include <entt/single_include/entt/entt.hpp>

#ifdef PLATFORM_WIN64
#pragma warning (pop)
#elif PLATFORM_PS4
#pragma GCC diagnostic pop
#endif

#include <Components/TransformComponent.h>
#include <Components/NameComponent.h>
#include <Components/ModelComponent.h>
#include <Components/CameraComponent.h>
#include <Components/LightComponent.h>
#include <Core/Audio/AudioComponent.h>

namespace Th
{
	using namespace Th::Components;

	class Systems
	{
	public:

		Systems() = default;
		virtual ~Systems() = default;

		/// <summary>
		/// Is being called every frame.
		/// </summary>
		virtual void OnUpdate(float) = 0;

		/// <summary>
		/// Add current system to the register of systems.
		/// This will ensure that the system's "Update" will be called.
		/// </summary>
		void Register();

		/// <summary>
		/// </summary>
		/// <returns>entity register from the worldmanager containing all information about the components and entities in the world. 
		/// Entities and components can be fetched, created or removed from here.</returns>
		entt::registry& GetEntityRegister();

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

		/// <summary>
		/// Increase the pool size of a specified component.
		/// </summary>
		/// <typeparam name="T">Component type.</typeparam>
		template<typename  T>
		void Reserve(int a_Size);

		/// <summary>
		/// Increase the pool of the entity register.
		/// </summary>
		void Reserve(int a_Size);

		/// <summary>
		/// </summary>
		/// <returns>The count of entities in the entity register.</returns>
		size_t GetEntityCount();

		/// <summary>
		/// </summary>
		/// <returns>The count of the capacity of the entity register.</returns>
		size_t GetEntityCapacity();

		/// <summary>
		/// Create a new entity with a name and transform.
		/// </summary>
		/// <returns></returns>
		entt::entity CreateEntity(const std::string& a_Name, Th::Components::TransformComponent& a_Transform);

		/// <summary>
		/// Check whether given entity has given type of component.
		/// </summary>
		/// <typeparam name="T">Component</typeparam>
		/// <returns></returns>
		template<typename T>
		bool HasComponentOfType(entt::entity a_Entity);

		/// <summary>
		/// Check whether given entity has given types of components.
		/// </summary>
		/// <typeparam name="...T">Components</typeparam>
		/// <returns></returns>
		//template<typename... T>
		//bool HasComponentsOfType(entt::entity a_Entity);

		/// <summary>
		/// Shrink the given pool capacity to the actual used size.
		/// </summary>
		/// <typeparam name="T">The component type to shrink the pool size for.</typeparam>
		template<typename T>
		void Shrink();
	};

	template<typename  T>
	void Systems::RemoveComponent(entt::entity a_Entity)
	{
		GetEntityRegister().remove_if_exists<T>(a_Entity);
	}

	template<typename  T>
	void Systems::Reserve(int a_Size)
	{
		GetEntityRegister().reserve<T>(a_Size);
	}

	template<typename T>
	void Systems::Shrink()
	{
		GetEntityRegister().shrink_to_fit<T>();
	}

	template <typename T>
	bool Systems::HasComponentOfType(entt::entity a_Entity)
	{
		return GetEntityRegister().has<T>();
	}

	//template<typename... T>
	//bool Systems::HasComponentsOfType(entt::entity a_Entity)
	//{
	//	return GetEntityRegister().any<T>();
	//}
}