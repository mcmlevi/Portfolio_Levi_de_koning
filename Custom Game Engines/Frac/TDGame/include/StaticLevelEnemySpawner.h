#pragma once
#include "StaticLevelInteractable.h"
struct StaticLevelEnemySpawner : public StaticLevelInteractable {
public:
	int m_enemySpawnType;
	float m_enemySpawnDelay;
private:
};