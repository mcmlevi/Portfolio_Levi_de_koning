#pragma once
#include <Flecs/flecs.h>

#include <cstdint>
#include <string>

typedef uint64_t EntityHandle;

namespace Frac
{
	/**
 * \brief helper class for wrapping a flecs::column 
 * \tparam ComponentType the type of the component by default you should make the component type const
 */
template<typename ComponentType>
class Column
{
public:
	Column(flecs::column<ComponentType> column) :m_column{ column }
	{
		
	}
	ComponentType* operator->()
	{
		return m_column.operator->();
	}
	bool isSet() const
	{
		return m_column.is_set();
	}
private:
	flecs::column<ComponentType> m_column;
};
	/**
 * \brief Wrapper class for the itterator that will allow you to grab components from the itterator
 */
class It
{
public:
	template<typename component>
	Frac::Column<component> column(int32_t index)
	{
		return Frac::Column<component>(m_it.column<component>(index));
	}
	It(flecs::iter& it) :m_it{ it } {}
	flecs::iter m_it;
};
	
template <typename ...components>
class Query
{
public:
	template <typename ...args>
	Query(flecs::world* world, const std::string& optionalArguments = "");
	template <typename ...args>
	void Iter(std::function<void(It, uint64_t, args...)> func);
	template <typename ...args>
	void Each(std::function<void(EntityHandle ,args...)> func);
private:
	flecs::query<components...> m_query;
};

template <typename ... components>
template <typename ... args>
::Frac::Query<components...>::Query(flecs::world* world, const std::string& optionalArguments) :
	m_query{ world->query<components...>(optionalArguments.c_str()) }
{

}

	template <typename ... components>
	template <typename ...args>
	void Query<components...>::Iter(std::function<void(It, uint64_t, args...)> func)
	{
		m_query.iter([func](flecs::iter iter, args&&... arguments)
			{
				It it{ iter };
				for (auto i : iter)
				{
					func(it, i, arguments...);
				}
			}
		);
	}

	template <typename ... components>
	template <typename ... args>
	void Query<components...>::Each(std::function<void(EntityHandle, args...)> func)
	{
		m_query.each([func](flecs::entity e, args&&... arguments)
			{
				func(e.id(), arguments...);
			}
		);
	}
}
