#pragma once
#include "EnemyComponent.h"
#include "Core/Systems.h"
#include "EnemySystem.h"
#include "LevelManager.h"
class WaveSystem :
    public Th::Systems
{
public:
	WaveSystem(EnemySystem&system, LevelManager&manager);
	~WaveSystem() = default;
	void Init();
	void OnUpdate(float) override;
	void SendWave(unsigned int wave);
	void Start();
	void CreateWaves(unsigned int amount = 5, unsigned int which = 1, unsigned int diffuculty = 1);
	void AddToWave(entt::entity);
	void RemoveFromList(entt::entity);
private:
	unsigned int m_CurrentWave;
	std::map<unsigned int, std::map<EnemyType, EnemyComponent>> m_EnemyData;
	std::map<unsigned int, unsigned int> m_Wave;
	std::vector<entt::entity> m_EnemiesInCurrentWave;
	int m_GoldBonus;
	EnemySystem* m_System;
	float m_Time;
	float m_SpawnTimer;
	float m_SpawnRate;
	unsigned int m_Spawned;
	unsigned int m_ToSpawn;
	bool m_go;
	float m_StartTimer;
	LevelManager* m_LevelManager;
	bool m_CanSpawn;
	float m_NextWaveTimer;
	float m_NextWaveIntervals;

	unsigned int m_Difficulty;
};

