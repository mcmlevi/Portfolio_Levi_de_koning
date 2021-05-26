#pragma once
#include "Core\Systems.h"
class ProjectileSystem :
    public Th::Systems
{
public:
	ProjectileSystem();
	~ProjectileSystem() = default;

	void OnUpdate(float) override;
private:
};

