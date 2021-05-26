#include <Thpch.h>
#include <Core/Systems.h>
#include <Core/WorldManager.h>

void Th::Systems::Register()
{
	Th::WorldManager::GetSingleton().RegisterSystem(this);
}

entt::registry& Th::Systems::GetEntityRegister()
{
	return WorldManager::GetSingleton().GetEntityRegister();
}

void Th::Systems::DeleteEntity(entt::entity a_Entity)
{
	Th::WorldManager::GetSingleton().DeleteEntity(a_Entity);
}

void Th::Systems::RemoveAllComponents(entt::entity a_Entity)
{
	GetEntityRegister().remove_all(a_Entity);
}

void Th::Systems::Reserve(int a_Size)
{
	GetEntityRegister().reserve(a_Size);
}

size_t Th::Systems::GetEntityCount()
{
	return GetEntityRegister().size();
}

size_t Th::Systems::GetEntityCapacity()
{
	return GetEntityRegister().capacity();
}

entt::entity Th::Systems::CreateEntity(const std::string& a_Name, Th::Components::TransformComponent& a_Transform)
{
	return WorldManager::GetSingleton().CreateEntity(a_Name, a_Transform);
}
