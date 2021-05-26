#pragma once
#include "Scene.h"
class fakeScene :
	public Scene
{
public:
	fakeScene();
	~fakeScene() = default;
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;
private:
	Image fake{ "assets/referenceImage.png",974,597,4 };
	Material fakeMat{ 0.f, 0.18f, 0.f, &fake };
};

