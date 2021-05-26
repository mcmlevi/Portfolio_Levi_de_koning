#include "pch.h"
#include "EnemyManager.h"
#include "Core/EngineCore.h"
#include "CoreRenderAPI/Components/Transform.h"
EnemyManager::EnemyManager(Frac::EngineConnector* connector, TileManager* tileManager) : m_connector(connector), m_tileManager(tileManager),
m_spawnerq{ m_connector->GetEngine().GetWorld().makeQuery<SpawnerComponent>() },
m_updateEnemyS{ m_connector->GetEngine().GetWorld().makeSystem<VelocityComponent, TOR::Transform, GridPositionComponent, DirectionComponent, EnemyComponent, HealthComponent>() },
m_spawnerS{ m_connector->GetEngine().GetWorld().makeSystem<SpawnerComponent, TOR::Transform, GridPositionComponent>() },
m_enemyq{ m_connector->GetEngine().GetWorld().makeQuery<EnemyComponent>() }
{
	m_totalEnemiesSpawned = 0;
	m_totalEnemiesToSpawn = 0;
	m_curWave = 0;
	m_maxWave = 0;
	m_totalSpawners = 0;
	m_enemiesCounter = 0;

	LOGINFO("Enemy Manager Initialized");
}

EnemyManager::~EnemyManager()
{
	m_connector = nullptr;
	m_tileManager = nullptr;
}

void EnemyManager::Enemy_MovementSystem()
{
	std::function<void(Frac::EntityHandle, VelocityComponent&, TOR::Transform&, GridPositionComponent&, DirectionComponent&, EnemyComponent&, HealthComponent&)> func
	{
		[this](Frac::EntityHandle e,VelocityComponent& velocity, TOR::Transform& transform, GridPositionComponent& gridPos, DirectionComponent& direction, EnemyComponent& enemy, HealthComponent& health)
		{
			Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
			

			if (enemy.m_hasTarget == false)
			{
				//Find Next Tile to move into
				const size_t curTileID = m_tileManager->GetTileID(gridPos.m_gridPosition);
				glm::vec2 nextPos = m_tileManager->GetNextTilePos(curTileID, enemy.m_patience);				
				
				m_tileManager->SetTileAvailability(gridPos.m_gridPosition, false, e);	//Set Current Tile to Occupied and Locked

				//Remained to the same pos -> decrease patience
				if (nextPos == gridPos.m_gridPosition)
				{
					transform.isDirty = false;

					enemy.m_timeElapsed += world.getDeltaTime();
					if (enemy.m_timeElapsed >= enemy.m_patienceTimer)
					{
						enemy.m_timeElapsed = 0.0f;
						enemy.m_patience--; 
						if (enemy.m_patience = -2)
						{
							enemy.m_patience = 1;
						}
						glm::clamp(enemy.m_patience, -1, 1);
					}
				}
				//Found a non-Occupied Tile
				if (nextPos != gridPos.m_gridPosition)
				{
					transform.isDirty = true;
					enemy.m_patience = 1;					
					enemy.m_nextGridPos = nextPos;
					enemy.m_target = m_tileManager->m_tileMap->GridPosToWorldPos(nextPos);	
					direction.m_direction = glm::normalize(enemy.m_target - transform.Position);
					m_tileManager->SetTileLocked(nextPos, true);							//Set Next Tile to Locked						
					enemy.m_hasTarget = true;
				}	
				
			}
			else
			{			
				const float margin = 0.1f;
				const glm::vec3 halfPoint = (glm::abs(enemy.m_target) + glm::abs(m_tileManager->m_tileMap->GridPosToWorldPos(gridPos.m_gridPosition))) / 2.0f;

				//Did reach Target Tile? (Goes into this once)
				if((IsInRange(transform.Position, halfPoint, margin) && (enemy.m_didReachTile == false)))
				{
					//Free previous Tile Occupier and set to current.
					enemy.m_didReachTile = true;

					m_tileManager->SetTileAvailability(gridPos.m_gridPosition, true, e); //Set previous tile to Available
					m_tileManager->SetTileAvailability(enemy.m_nextGridPos, false, e);	//Set new tile to Occupied and Locked.

					gridPos.m_gridPosition = enemy.m_nextGridPos;
					
					//Did Reach Base?
					const size_t curTileID = m_tileManager->GetTileID(enemy.m_nextGridPos);
					TileComponent* curTileComp = world.get<TileComponent>(m_tileManager->m_tileMap->m_tileEntities->at(curTileID));

					if (world.has(curTileComp->m_occupier) && curTileComp->m_floodFill.m_floodFillCost == 0)
					{
						//Base Takes Damage
						world.get_mutable<HealthComponent>(curTileComp->m_occupier)->m_health -= enemy.m_damageToBase;
						const uint baseHealth = world.get<HealthComponent>(curTileComp->m_occupier)->m_health;
						LOGINFO("Base took Damage! Remaining Health: %i ", baseHealth);

						//Remove Enemy from the World
						health.m_health = 0;
						return;
					}
				}
				//Check if we are going right or left		
				else if (IsInRange(transform.Position, enemy.m_target, margin) && (enemy.m_didReachTile == true))
				{
					enemy.m_hasTarget = false;
					enemy.m_didReachTile = false;
				}
				else
				{
					transform.Position += (direction.m_direction * (velocity.m_speed * world.getDeltaTime()));
				}							
			}		
		}
	};

	m_updateEnemyS = m_connector->GetEngine().GetWorld().makeSystem<VelocityComponent, TOR::Transform, GridPositionComponent, DirectionComponent, EnemyComponent, HealthComponent>(func, Frac::ePipelineStage::onUpdate, "UpdateEnemies");
	LOGINFO("Enemy Movement System Initialized");
}

bool EnemyManager::Enemy_IsAlive()
{
	bool isAlive = false;
	std::function<void(Frac::It, uint64_t, EnemyComponent*)> func = [&isAlive](Frac::It i, uint64_t index, EnemyComponent* enemy)
	{
		isAlive = true;
	};

	m_enemyq.Iter(func);
	return isAlive;
}

void EnemyManager::Spawner_InitQ()
{
	int spawners = 0;
	std::function<void(Frac::It, uint64_t, SpawnerComponent*)> func = [&spawners](Frac::It i, uint64_t index, SpawnerComponent* s)
	{
		spawners++;
	};
	m_spawnerq.Iter(func);

	m_totalSpawners = spawners;
	CalculateEnemiesToSpawn();
}

void EnemyManager::CalculateEnemiesToSpawn()
{
	//Temporary Function to calculate how many Enemies should be spawned per wave.
	m_totalEnemiesSpawned = 0;
	m_totalEnemiesToSpawn = 0;
	m_curWave++;

	if (m_curWave <= m_maxWave)
	{
		m_totalEnemiesToSpawn = m_curWave * 10 * m_totalSpawners;
		LOGINFO("New Wave Reached: %i " ,m_curWave);
	}
	else
	{
		m_totalEnemiesToSpawn = 0;

		LOGINFO("Max Waves Reached");
	}
	
}

void EnemyManager::Spawner_SpawnSystem()
{
	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

	std::function<void(Frac::EntityHandle, SpawnerComponent&, TOR::Transform&, GridPositionComponent&)> func
	{
		[this, &world](Frac::EntityHandle e,SpawnerComponent& s, TOR::Transform& t, GridPositionComponent& g)
		{
			s.m_timeElapsed += world.getDeltaTime();
			if ((s.m_timeElapsed >= s.m_spawnrate) && (m_totalEnemiesToSpawn > 0))
			{
				//Check if there is a valid Tile to spawn at
				glm::vec2 enemySpawnPos = GetTileToSpawn(g.m_gridPosition);

				if (enemySpawnPos.x != -1 && enemySpawnPos.y != -1)
				{
					//Set Values
					s.m_timeElapsed = 0.0f;
					glm::vec3 enemyWorldPos = m_tileManager->m_tileMap->GridPosToWorldPos(enemySpawnPos);

					glm::vec4 enemyColor = { 0,0,0,0 };
					glm::vec3 enemyDirection = glm::normalize(enemyWorldPos - t.Position);
					float enemySize = m_tileManager->m_tileMap->m_size;
					float enemySpeed = 0.0f;
					int enemyHealth = 0;

					//Spawn Enemy based on Type
					switch (s.m_spawnerType)
					{
					case EEnemyType::None:
						//do nothing
						break;

					case EEnemyType::Red:
						enemyColor = { 1,0,0,1 };
						enemySize = { enemySize/2.0f };
						enemySpeed = 1.0f;
						enemyHealth = 100;
						break;

					case EEnemyType::Green:
						enemyColor = { 0,1,0,1 };
						enemySize = { (enemySize/3.0f) };
						enemySpeed = 2.0f;
						enemyHealth = 50;
						break;

					case EEnemyType::Blue:
						enemyColor = { 0,0,1,1 };
						enemySize = { enemySize - (enemySize / 3.f) };
						enemySpeed = 0.5f;
						enemyHealth = 200;
						break;

					default:
						break;
					}

					if (s.m_spawnerType != EEnemyType::None)
					{
						m_enemiesCounter++;

						Frac::EntityHandle enemy = world.makeEntity<BoxComponent, GridPositionComponent, DirectionComponent, VelocityComponent, TOR::Transform, HealthComponent, EnemyComponent>
							("Enemy " + std::to_string(m_enemiesCounter),
								{ enemyColor, true },
								{ enemySpawnPos },
								{ enemyDirection },
								{ enemySpeed },
								{ enemyWorldPos, glm::vec3(enemySize) },
								{ enemyHealth },
								{ });

						m_totalEnemiesSpawned++;

						if (m_totalEnemiesSpawned == m_totalEnemiesToSpawn)
						{
							CalculateEnemiesToSpawn();
						}
					}
				}
			}
		}
	};

	m_spawnerS = world.makeSystem<SpawnerComponent, TOR::Transform, GridPositionComponent>(func, Frac::ePipelineStage::onUpdate, "Spawner_SpawnSystem");
}

glm::vec2 EnemyManager::GetTileToSpawn(glm::vec2 gridPos)
{
	const int allRows = m_tileManager->m_tileMap->m_rows;
	const int allCols = m_tileManager->m_tileMap->m_cols;

	size_t tileID;

	int xMin = (int)gridPos.x - 1;
	int yMin = (int)gridPos.y - 1;
	int xMax = (int)gridPos.x + 1;
	int yMax = (int)gridPos.y + 1;

	if (xMin < 0) xMin = 0;
	if (yMin < 0) yMin = 0;
	if (xMax > allRows) xMax = allRows;
	if (yMax > allCols) yMax = allCols;

	uint bestCost = 99999;
	glm::vec2 bestPosition = { -1,-1 };

	for (int x = xMin; x < xMax; x++)
	{
		for (int y = yMin; y < yMax; y++)
		{
			tileID = m_tileManager->GetTileID(glm::vec2(x, y));
			const TileComponent* curTile = m_connector->GetEngine().GetWorld().get<TileComponent>(m_tileManager->m_tileMap->m_tileEntities->at((tileID)));

			if (m_tileManager->CanMoveIntoTile(glm::vec2(x,y)) && (curTile->m_floodFill.m_floodFillCost < bestCost) && (curTile->m_states.m_isWalkable))
			{
				bestCost = curTile->m_floodFill.m_floodFillCost;
				bestPosition = { x,y };
			}
		}
	}

	return bestPosition;
}

glm::vec2 EnemyManager::GetEnemyInRange(uint range, glm::vec2 gridPos)
{
	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

	const int allRows = m_tileManager->m_tileMap->m_rows;
	const int allCols = m_tileManager->m_tileMap->m_cols;

	size_t tileID;

	int xMin = (int)gridPos.x - range;
	int yMin = (int)gridPos.y - range;
	int xMax = (int)gridPos.x + range;
	int yMax = (int)gridPos.y + range;
	
	if (xMin < 0) xMin = 0;
	if (yMin < 0) yMin = 0; 
	if (xMax > allRows) xMax = allRows;
	if (yMax > allCols) yMax = allCols;

	uint bestCost = 99999;
	glm::vec2 bestPosition = { -1,-1 };

	for (int x = xMin; x < xMax; x++)
	{
		for (int y = yMin; y < yMax; y++)
		{
			if (!((gridPos.x == x) && (gridPos.y == y)))
			{
				tileID = m_tileManager->GetTileID(glm::vec2(x, y));
				const TileComponent* curTile = world.get<TileComponent>(m_tileManager->m_tileMap->m_tileEntities->at((tileID)));

				if (m_tileManager->IsTileOccupied(glm::vec2(x, y)) && (curTile->m_floodFill.m_floodFillCost < bestCost))
				{
					if ((world.has(curTile->m_occupier) && world.has<EnemyComponent>(curTile->m_occupier)))
					{
						bestCost = curTile->m_floodFill.m_floodFillCost;
						bestPosition = { x,y };
					}
				}
			}
			
		}
	}

	return bestPosition;
}

bool EnemyManager::IsInRange(float low, float high, float curValue)
{
	return ((curValue - low) <= (high - low)); //Found at https://www.geeksforgeeks.org/how-to-check-whether-a-number-is-in-the-rangea-b-using-one-comparison/
}

bool EnemyManager::IsInRange(glm::vec3 curPos, glm::vec3 targetPos, float margin)
{
	bool isZ = false;
	bool isX = false;

	glm::vec3 target = glm::abs(targetPos);
	glm::vec3 current = glm::abs(curPos);
	
	float resultX = glm::distance(current.x, target.x);
	float resultY = glm::distance(current.z, target.z);

	if ((resultX <= margin) && (resultY <= margin))
	{
		return true;
	}

	return false;
}

void EnemyManager::SetMaxWave(int value)
{
	if (value <= 0)
	{
		LOGERROR("Tried to set a negative value to the Max Wave: %i ", value);
		m_maxWave = 1;
	}
	else
	{
		m_maxWave = value;
	}
	
}

