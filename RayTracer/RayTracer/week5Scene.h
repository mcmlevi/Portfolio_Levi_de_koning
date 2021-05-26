#pragma once
#include "Scene.h"
class week5Scene: public Scene
{
public:
	week5Scene();
	virtual  ~week5Scene() = default;
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;
};

