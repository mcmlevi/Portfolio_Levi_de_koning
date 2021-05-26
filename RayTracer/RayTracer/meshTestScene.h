#pragma once
#include "Scene.h"
class meshTestScene :
	public Scene
{
public:
	meshTestScene();
	~meshTestScene() override;
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;
		
private:
	Material reflectiveDragon{0.0f,0.18f,1.5f};
	Material refraction{ 0.0f,0.18f,1.5f };
	Material OpaqueDragon{ 0.f,0.18f,0.f };
};

