#pragma once
#include "IEntityCreator.h"
#include "Components.h"
class PlayerEntityCreator : public IEntityCreator
{
public:
	PlayerEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* connector);
	~PlayerEntityCreator();
	void CreatePlayerBase(int id, glm::vec3 position, glm::vec3 rotation, float coreHealth, float coreTargetData);
	void CreateTower(int id, glm::vec3 position, glm::vec3 rotation, float fireRate, float BulletSpeed); //TODO create version that supports bullet types
	glm::vec2 GetBasePos() const;

private:
	Frac::ECSWrapper* m_world;
	int createdTower = 0;
	glm::vec2 m_basePos = glm::vec2(0);
};
