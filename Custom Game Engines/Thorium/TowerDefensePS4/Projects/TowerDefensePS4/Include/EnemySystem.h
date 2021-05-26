#pragma once
#include "PlayerSystem.h"
#include "Core\Systems.h"
#include "LevelManager.h"

//class PlayerSystem;
class WaveSystem;

enum class EnemyType
{
	Red,
	RedWG,
	Green,
	GreenWG,
	Yellow,
	YellowWG,
	Purple,
	PurpleWG,
};
class EnemySystem :
	public Th::Systems
{
public:
	EnemySystem(LevelManager* levelManager);
	~EnemySystem() = default;
	void Initialize(PlayerSystem* sys, WaveSystem& waveSys);
	void CreateEnemy(float speed, int health, glm::vec3 pos, EnemyType enemytype, int gold);
	void OnUpdate(float) override;
private:
	std::vector<entt::entity> m_Enemies;
	PlayerSystem* m_System;
	LevelManager* m_LevelManager;
	WaveSystem* m_WaveSystem;
	glm::vec3 moveto = glm::vec3(2, 5, 4);
	glm::vec3 next = glm::vec3(8, 5, 4);
	float m_Time;
	float m_StartTimer;
	bool m_Initialized = false;
};
