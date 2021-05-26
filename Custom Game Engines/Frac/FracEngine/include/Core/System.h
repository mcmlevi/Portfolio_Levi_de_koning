// Copyright 2021 Dataflow
#pragma once
#include <Flecs/flecs.h>

#include <string>

namespace Frac
{
typedef uint64_t EntityHandle;
enum class ePipelineStage
{
	onLoad,
	postLoad,
	preUpdate,
	onUpdate,
	onValidate,
	postUpdate,
	preStore,
	onStore,
	onAdd,
	onSet,
	OnRemove,
};
template <typename ...components>
class System
{
public:
	template <typename ...args>
	System(flecs::world* world, std::function<void(EntityHandle, args...)> func, ePipelineStage stage, const std::string& name);
	System(flecs::world* world);
	System(const System<components...>& sys);
	/**
	 * \brief Enable the system
	 */
	void enable();
	/**
	 * \brief disable the system
	 */
	void disable();
private:
	const flecs::entity_t& translateStage(ePipelineStage stage);
	flecs::system<components...> m_system;
};
	
template <typename ... components>
template <typename ... args>
::Frac::System<components...>::System(flecs::world* world, std::function<void(EntityHandle, args...)> func, ePipelineStage stage, const std::string& name) :
	m_system{ world->system<components...>(name.c_str()) }
{
	const flecs::entity_t& kind = translateStage(stage);
	m_system.kind(kind);
	m_system.each([func](flecs::entity e, args&&... arguments)
		{
			func(e.id(), arguments...);
		});
}

template <typename ... components>
System<components...>::System(flecs::world* world):m_system{world->system<components...>()}
{
	
}

template <typename ... components>
System<components...>::System(const System<components...>& sys) : m_system(sys.m_system)
{
	
}


template <typename ... components>
void System<components...>::enable()
{
	m_system.enable();
}

template <typename ... components>
void System<components...>::disable()
{
	m_system.disable();
}

template <typename ... components>
const flecs::entity_t& System<components...>::translateStage(ePipelineStage stage)
{
	switch (stage)
	{
	case ePipelineStage::onLoad:
		return flecs::OnLoad;
	case ePipelineStage::postLoad:
		return flecs::PostLoad;
	case ePipelineStage::preUpdate:
		return flecs::PreUpdate;
	case ePipelineStage::onUpdate:
		return flecs::OnUpdate;
	case ePipelineStage::onValidate:
		return flecs::OnValidate;
	case ePipelineStage::postUpdate:
		return flecs::PostUpdate;
	case ePipelineStage::preStore:
		return flecs::PreStore;
	case ePipelineStage::onStore:
		return flecs::OnStore;
	case ePipelineStage::onAdd:
		return flecs::OnAdd;
	case ePipelineStage::onSet:
		return flecs::OnSet;
	case ePipelineStage::OnRemove:
		return  flecs::OnRemove;
	default:
		return flecs::OnUpdate;
	}
}
}//namespace Frac
