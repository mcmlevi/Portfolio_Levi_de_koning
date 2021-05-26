// Copyright 2021 Dataflow
#pragma once
#include <Flecs/flecs.h>
#include <Core/System.h>
#include <Core/Query.h>

#include <cstdint>

#include <cassert>
#include <functional>
#include <string>
#include <unordered_map>
#include <memory>
#include <utility>
#include <vector>
namespace Frac
{
typedef uint64_t EntityHandle;
typedef uint64_t Trait;
constexpr EntityHandle NullHandle{ 0 };
	
class ECSWrapper
{
public:
	ECSWrapper();
	~ECSWrapper();
	/**
	 * \brief Update the ECS world needs to be called once a frame
	 */
	void update();
	/**
	 * \brief Create an entity
	 * \param name optional name paramater to name the entity
	 * \return the handle to the entity
	 */
	const EntityHandle makeEntity(const std::string& name = "");
	float getDeltaTime();
	template<typename... components>
	const EntityHandle makeEntity(const std::string& name,components&&... componentsToSet);
	/**
	 * \brief add a component to the entity
	 * \tparam T the component type to add
	 * \param entityToAddTo the handle to the entity to add the component to
	 * \return a reference to the ECSWrapper so you can chain commands.
	 */
	template<typename T>
	ECSWrapper& add(const EntityHandle entityToAddTo);
	
	/**
	 * \brief Sets a component on an entity
	 * \tparam T type of component to set
	 * \param entityToSetTo the entity to add the component to
	 * \param component the component to add
	 * \return a reference to the ECSWrapper so you can chain commands.
	 */
	template<typename T>
	ECSWrapper& set(const EntityHandle entityToSetTo, T&& component);
	/**
	 * \brief Get a component from a specific entity
	 * \tparam T the type of component to get
	 * \param entityToGetFrom the entity to get the component from
	 * \return a reference to the component
	 */
	template<typename T>
	T* get(const EntityHandle entityToGetFrom);
	/**
	 * \brief Same as get but gets a mutable version instead
	 * \tparam T 
	 * \param entityToGetFrom 
	 * \return 
	 */
	template<typename T>
	T* get_mutable(const EntityHandle entityToGetFrom);
	template<typename T>
	T& get_ref(const EntityHandle entityToGetFrom);
	/**
	 * \brief Check if an entity has a specific component
	 * \tparam T the type of component to check
	 * \param entityToCheckOn the entity to check on.
	 */
	template<typename T>
	bool has(const EntityHandle entityToCheckOn);
	/**
	 * \brief Check if the world has a specific entity
	 * \param entityToCheck the handle to the entity to check
	 */
	bool has(const EntityHandle entityToCheck);
	/**
	 * \brief make a new trait
	 * \return the handle to the new trait
	 */
	const Trait makeTrait(const std::string& name = "");
	/**
	 * \brief Add a trait to a spefic component
	 * \param a_trait the trait to add
	 * \param a_entity the entity to add the trait to
	 * \return a reference to the ECSWrapper so you can chain commands.
	 */
	ECSWrapper& addTrait(const EntityHandle entityToAddTraitTo, const Trait trait, const EntityHandle traitEntity);
	/**
	 * \brief Remove a trait from a specific entity
	 * \param a_traitToRemove the trait to remove
	 * \param a_entity the entity to remove the trait from
	 * \return a reference to the ECSWrapper so you can chain commands.
	 */
	ECSWrapper& removeTrait(const EntityHandle entityToRemoveTraitFrom, const Trait trait, const EntityHandle traitEntity);
	/**
	 * \brief 
	 * \param a_trait 
	 * \param a_entity 
	 * \return flecs::query<args...>
	 */
	bool hasTrait(const EntityHandle EntityToCheckTraitOn, const Trait trait, const EntityHandle traitEntity);
	/**
	 * \brief make a system that will be automatically run in certain pipeline stages
	 * \tparam components the components the system will loop over
	 * \tparam args the arguments of the lambda so Pos&, const Pos& ect
	 * \param func the function to call
	 * \param stage the stage of the pipeline the system should run in
	 * \param name the name of the system
	 * \return the generated system wrapper
	 */
	template <typename ...components,typename ...args>
	Frac::System<components...> makeSystem(std::function<void(EntityHandle, args ...)> func,
	                                       ePipelineStage stage = ePipelineStage::onUpdate,
	                                       const std::string& name = "");
	template <typename ...components>
	Frac::System<components...> makeSystem();
	template <typename ...components>
	Frac::Query<components...> makeQuery(const std::string& optionalArguments = "");
	/**
	 * \brief Remove a component from an entity
	 * \tparam T the component type to remove
	 * \param entityToRemoveComponentFrom the handle to the entity to remove the component from
	 */
	template<typename T>
	ECSWrapper& remove(const EntityHandle entityToRemoveComponentFrom);
	/**
	 * \brief Remove an entity from the world
	 * \param entityToRemove the entity to remove
	 */
	ECSWrapper& remove(const EntityHandle entityToRemove);
	/**
	 * \brief Get the parent of the specified entity will return a nullhandle if none is found
	 * \tparam T the components the parent should have for example I want the parent entity with the following components, position and model
	 * \param childEntity the entity to get the parent of.
	 * \return the handle to the found parent
	 */
	template<typename ...T>
	const EntityHandle getParent(const EntityHandle childEntity);
	/**
	* \brief Get the parent of the specified entity will return a nullhandle if none is found
	* \param childEntity the entity to get the parent of.
	* \return the handle to the found parent
	*/
	const EntityHandle getParent(const EntityHandle childEntity);
	
	/**
	 * \brief Check if 2 components have a parent-child relationship
	 * \param child the parent entity
	 * \param parent the parent entity
	 */
	const bool hasParent(const EntityHandle child, const EntityHandle parent);
	
	/**
	 * \brief Add a child to a parent
	 * \param child entity handle
	 * \param parent entity handle
	 * */
	ECSWrapper& addChild(const EntityHandle child, const EntityHandle parent);
	/**
	 * \brief Remove a child from a parent
	 * \param parent entity handle
	 * \param child entity handle 
	 */
	ECSWrapper& removeChild(const EntityHandle parent, const EntityHandle child);
	/**
	 * \brief Make a prefab
	 * \param name the name of the prefab
	 * \return the entity handle to the prefab
	 */
	const EntityHandle makePrefab(const std::string& name = "");
	template<typename... components>
	const EntityHandle makePrefab(const std::string& name,components&&... componentsToSet);
	/**
	 * \brief Generate a new entity based of a prefab
	 * \param prefab the prefab to base the new entity of
	 * \param name the name of the entity
	 * \return the handle to the newly generated entity
	 */
	const EntityHandle makeFromPrefab(const EntityHandle prefab,const std::string& name = "");

	/**
	 * \brief Add prefab components to existing entity
	 * \param prefab the prefab to add to existing entity
	 * \param handle to existing entity prefab values will be added to
	 * \return the handle to the appended entity
	 */
	const EntityHandle AddInstancedPrefabToEntity(const EntityHandle prefab, const EntityHandle entityPrefabShouldBeAddedTo);
	/**
	 * \brief Same as the default make from prefab but this one set the specified components
	 * \tparam components the components you want to set.
	 */
	template<typename... components>
	const EntityHandle makeFromPrefab(const EntityHandle prefab, const std::string& name,
	                                  components&&... componentsToSet);
	const std::string getName(const EntityHandle entity);
	template<typename component>
	int count();
	std::vector<EntityHandle> getChilderen(EntityHandle entity);
	std::string getType(EntityHandle entity);
	template <typename T>
	void add_owned(EntityHandle entity);
private:
	// Some helper functions to recursively call template arguments
	template <class F, class First, class... RestOfComponents>
	void doFor(F f, First first, RestOfComponents&& ... rest);
	template <class F>
	void doFor(F f);

	template <class... Args>
	void setRecursive(const EntityHandle e,Args&& ... args);
	bool findEntity(uint64_t id) const;
	std::unique_ptr<flecs::world> m_registry;
};

template <typename ... components>
const EntityHandle ECSWrapper::makeEntity(const std::string& name, components&&... componentsToSet)
{
	const EntityHandle e = makeEntity(name);
	setRecursive(e, std::forward<components>(componentsToSet)...);
	return e;
}

template <typename T>
ECSWrapper& ECSWrapper::add(const EntityHandle entityToAddTo)
{
	m_registry->add(m_registry->entity(entityToAddTo).add<T>());
	return *this;
}
template <typename T>
ECSWrapper& ECSWrapper::set(const EntityHandle entityToSetTo, T&& component)
{
	m_registry->set(m_registry->entity(entityToSetTo).set(std::move(component)));
	return *this;
}
template <typename T>
T* ECSWrapper::get(const EntityHandle entityToGetFrom) 
{
	assert(findEntity(entityToGetFrom));
	return m_registry->entity(entityToGetFrom).get<T>();
}

template <typename T>
T* ECSWrapper::get_mutable(const EntityHandle entityToGetFrom)
{
	assert(findEntity(entityToGetFrom));
	return m_registry->entity(entityToGetFrom).get_mut<T>();
}

template <typename T>
T& ECSWrapper::get_ref(const EntityHandle entityToGetFrom)
{
	assert(findEntity(entityToGetFrom));
	return m_registry->entity(entityToGetFrom).get_ref<T>();
}

template <typename T>
bool ECSWrapper::has(const EntityHandle entityToCheckOn)
{
	assert(findEntity(entityToCheckOn));
	return m_registry->entity(entityToCheckOn).has<T>();
}
template <typename T>
ECSWrapper& ECSWrapper::remove(const EntityHandle entityToRemoveComponentFrom)
{
	assert(findEntity(entityToRemoveComponentFrom));
	flecs::entity e = m_registry->entity(entityToRemoveComponentFrom);
	e.remove<T>();
	return *this;
}
template <typename ...components, typename ...args>
Frac::System<components...> ECSWrapper::makeSystem(std::function<void(EntityHandle, args...)> func, ePipelineStage stage, const std::string& name)
{
	return Frac::System<components...>{m_registry.get(), func, stage,name};
}

template <typename ... components>
Frac::System<components...> ECSWrapper::makeSystem()
{
	return Frac::System<components...>{m_registry.get()};
}

template <typename ... components>
Frac::Query<components...> ECSWrapper::makeQuery(const std::string& optionalArguments)
{
	return Frac::Query<components...>{m_registry.get(), optionalArguments};
}

template <typename ...T>
const EntityHandle ECSWrapper::getParent(const EntityHandle childEntity)
{
	assert(findEntity(childEntity));
	flecs::entity e = m_registry->entity(childEntity);
	flecs::entity p = e.get_parent<T...>();
	if (p)
	{
		return p.id();
	}
	return NullHandle;
}

template <typename ... components>
const EntityHandle ECSWrapper::makePrefab(const std::string& name, components&&... componentsToSet)
{
	EntityHandle prefab = makePrefab(name);
	setRecursive(prefab, std::forward<components>(componentsToSet)...);
	return prefab;
}

template <typename ... components>
const EntityHandle ECSWrapper::makeFromPrefab(const EntityHandle prefab, const std::string& name,
	components&&... componentsToSet)
{
	const EntityHandle e = makeFromPrefab(prefab, name);
	setRecursive(e, std::forward<components>(componentsToSet)...);
	return e;
}

template <typename component>
int ECSWrapper::count()
{
	return m_registry->count<component>();
}

template <typename T>
void ECSWrapper::add_owned(EntityHandle entity)
{
	m_registry->entity(entity).add_owned<T>();
}

template <class F, class First, class ... Rest>
void ECSWrapper::doFor(F f, First first, Rest&& ... rest)
{
	f(first);
	doFor(f, std::forward<Rest>(rest)...);
}

template <class F>
void ECSWrapper::doFor(F f)
{
	// Parameter pack is empty.
}

template <class ... Args>
void ECSWrapper::setRecursive(const EntityHandle e, Args&& ... args)
{
	doFor([this,e](auto&& arg)
	{
			this->set(e, arg);
	}, std::forward<Args>(args)...);
}
} // namespace Frac
