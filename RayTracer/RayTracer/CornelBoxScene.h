#pragma once
#include "Scene.h"
class CornelBoxScene :
	public Scene
{
public:
	CornelBoxScene();
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;
	
};

