#pragma once
#include "IEntityCreator.h"
#include "Components.h"
class EnemyEntityCreator : public IEntityCreator
{
public:
	EnemyEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* connector);
	~EnemyEntityCreator();

	void CreateEnemy(int id, glm::vec3 gridpositon, glm::vec3 rotation, int enemyLevel);
	void CreateSpawner(int id, glm::vec3 gridPosition, glm::vec3 rotation, int enemySpawnType, float enemySpawnDelay);
	EEnemyType GetEnemySpawnType(int type);
private:
	Frac::ECSWrapper* m_world;
	int createdSpawners = 0;
};
