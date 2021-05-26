#pragma once
#include "Components.h"
#include "Core/EngineConnector.h"
#include "TileManager.h"

class EnemyManager
{
public:
	EnemyManager(Frac::EngineConnector* connector, TileManager* tileManager);

	~EnemyManager();

	void Enemy_MovementSystem();
	bool Enemy_IsAlive();

	void Spawner_InitQ();	
	void Spawner_SpawnSystem();

	glm::vec2 GetTileToSpawn(glm::vec2 gridPos);

	glm::vec2 GetEnemyInRange(uint range, glm::vec2 gridPos);

	bool IsInRange(float low, float high, float curValue);
	bool IsInRange(glm::vec3 curPos, glm::vec3 targetPos, float margin);
	void SetMaxWave(int value);

	//Systems
	Frac::System<VelocityComponent, TOR::Transform, GridPositionComponent, DirectionComponent, EnemyComponent, HealthComponent> m_updateEnemyS;
	Frac::System< SpawnerComponent, TOR::Transform, GridPositionComponent> m_spawnerS;

	int m_curWave;
	int m_maxWave;
	int m_totalEnemiesToSpawn;
	int m_totalEnemiesSpawned;

private:
	void CalculateEnemiesToSpawn();

	Frac::EngineConnector* m_connector;
	TileManager* m_tileManager;
	
	int m_totalSpawners;
	
	uint m_enemiesCounter;

	Frac::Query<SpawnerComponent> m_spawnerq;
	Frac::Query<EnemyComponent> m_enemyq;
};

