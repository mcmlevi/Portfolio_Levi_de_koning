#pragma once
#include "Scene.h"
class week8Reference :
	public Scene
{
public:
	week8Reference();
	~week8Reference() = default;
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;

private:
	Image t_crate{ "assets/crate.png",480,480,4 };
	Image n_crate{ "assets/crateNormal.png",480,480,4 };
	Material m_crate{ 0,0.18f,0,&t_crate,&n_crate };
};

