#include <Flecs\flecs.h>

typedef struct Velocity {
	float x, y;
}Velocity;
#pragma once

template 
<typename ... BeautifulList>
class System
{	
	public:
	System() = default;


	std::function<void(flecs::entity,BeautifulList...)> SystemFunction;
	
	//void addSystemToEngine(System* sys);
	private:

	ecs_entity_t flecsPhase;
};



class TestSystem : public System<Velocity&>
{
	public:
	TestSystem();
	
	void Action(flecs::entity e, Velocity& p);

	

};

