#include "pch.h"
#include "WaveSystem.h"

#include "Core/MessageLogger.h"
#include "Utility/Timer.h"


WaveSystem::WaveSystem(EnemySystem& system,LevelManager&manager) : m_SpawnRate(0), m_SpawnTimer(0), m_CurrentWave(0), m_Spawned(0),m_go(false), m_StartTimer(0),m_NextWaveIntervals(0), m_NextWaveTimer(0)
{
	Register();

	m_SpawnRate = .5f;
	m_StartTimer = 1;
	m_NextWaveIntervals = 10.0f;
	m_System = &system;
	m_LevelManager = &manager;

	unsigned int index;
	EnemyType type;
	EnemyComponent component;

	index = 2;
	type = EnemyType::Yellow;
	component.Initialize(5, 20, 2);

	std::map<EnemyType, EnemyComponent> inner;
	inner.insert(std::pair(type, component));
	m_EnemyData.insert(std::make_pair(index, inner));

	index = 1;
	type = EnemyType::Green;
	component.Initialize(2, 25, 1);

	inner.insert(std::make_pair(type, component));
	m_EnemyData.insert(std::make_pair(index, inner));

	index = 3;
	type = EnemyType::Red;
	component.Initialize(1, 100, 3);

	inner.insert(std::make_pair(type, component));
	m_EnemyData.insert(std::make_pair(index, inner));
}

void WaveSystem::OnUpdate(const float dt)
{
	if (m_go)
	{
		m_SpawnTimer += dt;		
		if (m_SpawnTimer >= m_SpawnRate && m_CanSpawn)
		{
			SendWave(m_CurrentWave);
			m_SpawnTimer -= m_SpawnRate;
		}
		if (m_Spawned >= m_ToSpawn)
		{
			m_CanSpawn = false;
			m_Spawned = 0;
		}		
		if (!m_CanSpawn && m_EnemiesInCurrentWave.empty())
		{
			m_NextWaveTimer += dt;
			if (m_NextWaveTimer > m_NextWaveIntervals)
			{
				m_CanSpawn = true;
				m_CurrentWave++;
				//printf("%i", m_CurrentWave);
				m_NextWaveTimer = 0;
			}
		}	
	}
}

void WaveSystem::SendWave(unsigned int wave)
{
	auto it = m_Wave.find(wave);
	if (it != m_Wave.end())
	{
		unsigned int WhichEnemyToSpawn= it->second;	
		auto it2 = m_EnemyData.find(WhichEnemyToSpawn);
		if (it2 != m_EnemyData.end())
		{
			auto enemySecond = it2->second.begin();

			EnemyType type = enemySecond->first;
			EnemyComponent comp = enemySecond->second;

			int multiplier = 1 + wave* 0.02f * m_Difficulty;
			int goldMultiplier = 1 + wave* 0.01f * m_Difficulty;
			m_SpawnTimer = 0;
			Th::TransformComponent& StartPos = GetEntityRegister().get<Th::TransformComponent>(m_LevelManager->m_EnemyBeginTile->m_TileEntity);
			glm::vec3 startPosFixed = StartPos.GetPosition();
			m_System->CreateEnemy(comp.GetSpeed(), comp.GetHealth() * multiplier, startPosFixed, type, comp.GetWorth() *goldMultiplier);
			m_Spawned++;
		}
		else
		{
			LOGWARNING("Invalid Enemy Index");
		}
	}
	else
	{
		LOGWARNING("Invalid Wave index");
	}
	
}

void WaveSystem::Start()
{
	m_CurrentWave = 1;
	m_go = true;
	m_CanSpawn = true;
}


void WaveSystem::CreateWaves(unsigned int amount, unsigned int which, unsigned int difficulty )
{
	m_Difficulty = difficulty;
	unsigned int max = m_EnemyData.size();
	unsigned int Amounttospawn = amount;
	unsigned int whoToSpawn = which;
	m_ToSpawn = Amounttospawn;

	for (unsigned int i = 0; i < 999; i++)
	{
		Amounttospawn += i;
		m_Wave.insert(std::make_pair(i + 1, whoToSpawn));
		if (whoToSpawn >= max)
		{
			whoToSpawn = 0;
		}
		whoToSpawn += 1;
	}
	Start();
}

void WaveSystem::AddToWave(entt::entity ent)
{
	m_EnemiesInCurrentWave.push_back(ent);
}

void WaveSystem::RemoveFromList(entt::entity ent)
{
	auto it = std::find(m_EnemiesInCurrentWave.begin(), m_EnemiesInCurrentWave.end(), ent);
	if (it != m_EnemiesInCurrentWave.end())
	{
		m_EnemiesInCurrentWave.erase(it);
	}
}
