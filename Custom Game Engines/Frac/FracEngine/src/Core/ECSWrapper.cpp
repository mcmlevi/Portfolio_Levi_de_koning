#include "pch.h"
#include "Core/ECSWrapper.h"


namespace Frac
{
	ECSWrapper::ECSWrapper() :
		m_registry{ std::make_unique<flecs::world>() }
	{

	}

	ECSWrapper::~ECSWrapper()
	= default;

	void ECSWrapper::update()
	{
#if defined (PLATFORM_WINDOWS)
		m_registry->progress();
#else 
		
#endif
		
	}

	const EntityHandle ECSWrapper::makeEntity(const std::string& name)
	{
		flecs::entity e;
		//Create entity and insert it in the map
		if(name.empty())
			e = m_registry->entity();
		else
			e = m_registry->entity(name.c_str());
		return e.id();
	}

	float ECSWrapper::getDeltaTime()
	{
#if defined (PLATFORM_WINDOWS)
		const ecs_world_info_t* stats = ecs_get_world_info(m_registry->c_ptr());
		return stats->delta_time;
#else // fixed deltaTime for other platforms as the gettime from flecs only works on Windows
		return 0.02f;
#endif
	}

	bool ECSWrapper::has(const EntityHandle entity)
	{
		return findEntity(entity);
	}

	const Trait ECSWrapper::makeTrait(const std::string& name)
	{
		flecs::entity trait = m_registry->entity(name);
		return trait.id();
	}

	ECSWrapper& ECSWrapper::addTrait( EntityHandle entityToAddTraitTo,  Trait trait,  EntityHandle entity)
	{
		assert(findEntity(entityToAddTraitTo));
		assert(findEntity(trait));
		assert(findEntity(entity));
		flecs::entity p = m_registry->entity(entityToAddTraitTo);
		flecs::entity t = m_registry->entity(trait);
		flecs::entity e = m_registry->entity(entity);
		p.add_trait(t, e);
		return *this;
	}

	ECSWrapper& ECSWrapper::removeTrait(const EntityHandle entityToRemoveTraitFrom, const Trait trait, const EntityHandle entity)
	{
		assert(findEntity(entityToRemoveTraitFrom));
		assert(findEntity(trait));
		assert(findEntity(entity));
		flecs::entity p = m_registry->entity(entityToRemoveTraitFrom);
		flecs::entity t = m_registry->entity(trait);
		flecs::entity e = m_registry->entity(entity);
		p.remove_trait(t, e);
		return *this;
	}

	bool ECSWrapper::hasTrait(const EntityHandle EntityToCheckTraitOn, const Trait trait, const EntityHandle entity)
	{
		assert(findEntity(EntityToCheckTraitOn));
		assert(findEntity(entity));
		assert(findEntity(trait));
		flecs::entity p = m_registry->entity(EntityToCheckTraitOn);
		flecs::entity e = m_registry->entity(entity);
		flecs::entity t = m_registry->entity(trait);
		return p.has_trait(t,e);
	}

	ECSWrapper& ECSWrapper::addChild(const EntityHandle child, const EntityHandle parent)
	{
		assert(findEntity(child));
		assert(findEntity(parent));
		m_registry->entity(child).add_childof(m_registry->entity(parent));
		return *this;
	}

	ECSWrapper& ECSWrapper::removeChild(const EntityHandle parent, const EntityHandle child)
	{
		assert(findEntity(child));
		assert(findEntity(parent));
		flecs::entity p = m_registry->entity(parent);
		flecs::entity c = m_registry->entity(child);
		c.remove_childof(p);
		return *this;
	}

	ECSWrapper& ECSWrapper::remove(const EntityHandle entity)
	{
		assert(findEntity(entity));
		flecs::entity e{ m_registry->entity(entity) };
		e.destruct();
		return *this;
	}

	const EntityHandle ECSWrapper::getParent(const EntityHandle childEntity)
	{
		assert(findEntity(childEntity));
		flecs::entity e = m_registry->entity(childEntity);
		flecs::entity p = e.get_parent(flecs::entity::null());
		if (p)
		{
			return p.id();
		}
		return NullHandle;
	}

	const bool ECSWrapper::hasParent(const EntityHandle child, const EntityHandle parent)
	{
		assert(findEntity(child));
		assert(findEntity(parent));
		flecs::entity e = m_registry->entity(child);
		flecs::entity p = m_registry->entity(parent);
		return e.has_childof(p);
	}
	const EntityHandle ECSWrapper::makePrefab(const std::string& name)
	{
		flecs::entity e = m_registry->prefab(name.c_str());
		return e.id();
	}

	const EntityHandle ECSWrapper::makeFromPrefab(const EntityHandle prefab,const std::string& name)
	{
		assert(findEntity(prefab));
		flecs::entity e = m_registry->entity(prefab);
		EntityHandle handle = makeEntity(name);
		flecs::entity newEntity = m_registry->entity(handle);
		newEntity.add_instanceof(e);
		return handle;
	}

	const EntityHandle ECSWrapper::AddInstancedPrefabToEntity(const EntityHandle prefab, const EntityHandle entityPrefabShouldBeAddedTo)
	{
		assert(findEntity(prefab));
		assert(findEntity(entityPrefabShouldBeAddedTo));
		flecs::entity prefabWithOwnedData = m_registry->entity(prefab);
		flecs::entity passedEntityThatWantsMoreData = m_registry->entity(entityPrefabShouldBeAddedTo);
		passedEntityThatWantsMoreData.add_instanceof(prefabWithOwnedData);
		return entityPrefabShouldBeAddedTo;
	}

	const std::string ECSWrapper::getName(const EntityHandle entity)
	{
		return m_registry->entity(entity).name();
	}

	std::vector<EntityHandle> ECSWrapper::getChilderen(EntityHandle entity)
	{
		assert(findEntity(entity));
		flecs::entity e = m_registry->entity(entity);
		auto it = e.children();
		std::vector<EntityHandle> childeren{};
		for (auto& child : it)
		{
			for (int i = 0; i < child.count(); ++i)
			{
				childeren.push_back(child.entity(i).id());
			}
		}
		return childeren;
	}

	std::string ECSWrapper::getType(EntityHandle entity)
	{
		return m_registry->entity(entity).type().str();
	}

	bool ECSWrapper::findEntity(uint64_t id) const
	{
		flecs::entity e = m_registry->entity(id);
		return e.is_alive();
	}
}

