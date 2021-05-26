#include "Flecs\TestSystem.h"
#include <iostream>
 

TestSystem::TestSystem()
{
	SystemFunction = [this](flecs::entity e, Velocity& p) { this->Action(e, p); };
}

void TestSystem::Action(flecs::entity e, Velocity& p)
{
	p.x = -4;
	std::cout << "Beautiful entity with velocity " << p.x << " With Name: " << e.name() << std::endl;
}
