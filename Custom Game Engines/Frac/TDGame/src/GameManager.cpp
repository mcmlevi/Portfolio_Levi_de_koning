#include "pch.h"
#include "GameManager.h"
#include "Core/EngineConnector.h"
#include "Core/EngineCore.h"
#include "Glm/gtx/transform.hpp"
#include "CoreRenderAPI/Components/Transform.h"
#include "Graphics/Components/TextComponent.h"

#include "Core/Query.h"

#include "Audio/AudioManager.h"
#include "Audio/AudioComponent.h"

GameManager::GameManager(Frac::EngineConnector* connector, Frac::ECSWrapper* world)
	: m_connector(connector)
	,m_world(world)
	,m_healthCalculationS{ world->makeSystem<HealthComponent, GridPositionComponent>() }
	,m_turretFindTargetS{ world->makeSystem<TurretComponent, GridPositionComponent>() }
	,m_turretShootTargetS{ world->makeSystem<TurretComponent, DirectionComponent, TOR::Transform, GridPositionComponent>() }
	,m_bulletMovementS{ world->makeSystem<BulletComponent, DirectionComponent, TOR::Transform, VelocityComponent, GridPositionComponent>() }
	,m_tileGetLockedS{ world->makeSystem<TileComponent, BoxComponent, GridPositionComponent>() }
	,m_drawCubeS{ world->makeSystem<BoxComponent, TOR::Transform>() }
	,m_lastGameState(GameState::StateNone)
{
	m_isPaused = false;
	m_bulletCounter = 0;
	Init();
	LOGINFO("Game Manager Initialized");
}

GameManager::~GameManager()
{
	delete m_tileManager;
	m_sceneManager = nullptr;
}

void GameManager::Init()
{

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	InitDebugRenderer();
# endif

	m_inputSystem = &m_connector->GetEngine().GetInputSystem();
	m_sceneManager = &m_connector->GetEngine().GetSceneManager();
	
	//###Init Managers###
	
	//Tile manager
	m_tileManager = new TileManager(m_connector);
	m_levelEntityCreator = std::make_unique<LevelEntityCreator>(m_sceneManager,  m_tileManager, m_connector);
	m_playerEntityCreator = std::make_unique<PlayerEntityCreator>(m_sceneManager, m_tileManager, m_connector);
	m_enemyEntityCreator = std::make_unique<EnemyEntityCreator>(m_sceneManager, m_tileManager, m_connector);

	//Level manager
	m_levelManager = std::make_unique<LevelManager>(m_connector, m_tileManager, m_levelEntityCreator.get(), m_enemyEntityCreator.get(), m_playerEntityCreator.get());
	m_levelManager.get()->Init();
	m_levelManager.get()->LoadLevel(1); 

	
	
	//Enemy manager
	m_enemyManager = std::make_unique<EnemyManager>(m_connector, m_tileManager);
	m_enemyManager->SetMaxWave(m_levelManager->m_activeLevelData->m_levelWaves);

	//Player manager
	m_playerManager = std::make_unique<PlayerManager>(m_connector, m_tileManager, m_playerEntityCreator.get());
	m_playerManager->SetBuildCosts(50);

	// Camera manager
	m_cameraManager = std::make_unique<CameraManager>(m_connector->GetEngine());
	m_cameraManager->Initialize();
	m_cameraManager->CreateCamera(CameraManager::ECameraBehaviour::Static);
	m_cameraManager->CreateCamera(CameraManager::ECameraBehaviour::Noclip);
	m_camGame = m_cameraManager->CreateCamera(CameraManager::ECameraBehaviour::DefenseGrid, false, TOR::Transform({ glm::vec3(14.f, 15.f, -7.f) }), glm::vec3(0.25f, 0.f, -0.75f));
	m_camDemo = m_cameraManager->CreateCamera(CameraManager::ECameraBehaviour::Demo, true, 
		TOR::Transform({ glm::vec3(5.f, 15.f, 7.f) }), glm::vec3(5.f, 0.f, -7.75f));
	
	// UI manager
	m_uiManager = std::make_unique<UIManager>(
		m_connector->GetEngine()
		, *this);
	m_uiManager->Initialise(&m_gameState);
	
	//Spawn Entities into the World
	m_bulletCounter = 0;
	
	m_tileManager->RunFloodFillAlgorithm(); //needs to be called after we have a Base Entity

	//Init Systems	
	Health_CalculationSystem();
	Turret_FindTargetSystem();
	Turret_ShootTargetSystem();
	Bullet_MovementSystem();
	//Tile_GetLockedSystem();
	m_enemyManager->Spawner_InitQ();
	m_enemyManager->Spawner_SpawnSystem();
	//m_enemyManager->Enemy_UISystem();
	m_enemyManager->Enemy_MovementSystem();	
	
	m_connector->GetEngine().GetAudioManager().LoadBank("[Audio]Events.bank");
	m_connector->GetEngine().GetAudioManager().LoadBank("[Audio]Events.strings.bank");
	m_connector->GetEngine().GetAudioManager().LoadBank("[Audio]Music.bank");

	EntityHandle backgroundMusic = m_world->makeEntity("AudioSource_Test_Entity");
	m_world->add<TOR::Transform>(backgroundMusic);
	m_world->add<Frac::AudioSourceComponent>(backgroundMusic);
	m_connector->GetEngine().GetAudioManager().PlayEvent(*m_world->get<Frac::AudioSourceComponent>(backgroundMusic), "event:/SpaceMusic", 0.1f);

	//Set Game States
	m_gameState = GameState::StateMainMenu;
}

void GameManager::ManageSystems(bool activateSystem)
{
	if (activateSystem)
	{
		m_healthCalculationS.enable();
		m_turretFindTargetS.enable();
		m_turretShootTargetS.enable();
		m_bulletMovementS.enable();
		//m_TileGetLockedS.enable();
		m_enemyManager->m_spawnerS.enable();
		m_enemyManager->m_updateEnemyS.enable();
		//m_enemyManager->m_enemyUIS.enable();
		m_cameraManager->m_updateMainCameraS.enable();
	}
	else
	{
		m_healthCalculationS.disable();
		m_turretFindTargetS.disable();
		m_turretShootTargetS.disable();
		m_bulletMovementS.disable();
		//m_TileGetLockedS.disable();
		m_enemyManager->m_spawnerS.disable();
		m_enemyManager->m_updateEnemyS.disable();
		//m_enemyManager->m_enemyUIS.disable();
		m_cameraManager->m_updateMainCameraS.disable();
	}
}

void GameManager::UpdateGame()
{
	OnGameStateChange();
	
	switch (m_gameState)
	{
	case GameState::StateMainMenu:
	{
			
	}
		break;
	case GameState::StatePauseMenu:
	{
	
	}
		break;
	case GameState::StatePlaying:
	{
		if ((m_enemyManager-> m_totalEnemiesToSpawn == 0) && (m_enemyManager->m_curWave >= m_enemyManager->m_maxWave))
		{
			//Check if any enemies are alive, if not, you win
			if (!m_enemyManager->Enemy_IsAlive())
			{
				LOGINFO("YOU WIN!!");
				m_gameState = GameState::StateDefeat;
			}
			
		}

		m_uiManager->Update();
			
		m_playerManager->Update(GetSelectedGridPosition());
	}
		break;
	case GameState::StateVictory:
	{
		
	}
		break;
	case GameState::StateDefeat:
	{
		
	}
		break;
	}

}

glm::vec2 GameManager::GetSelectedGridPosition()
{
	EntityHandle camera = m_cameraManager->GetMainCameraEntity();
	glm::vec3 lookat = m_world->get<CameraManager::CameraTrait>(camera)->LookAt;
	return m_tileManager->m_tileMap->WorldPosToGridPos(lookat);
}

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
void GameManager::InitDebugRenderer()
{
	//Init
	m_debugRenderer = std::make_unique<Frac::DebugRenderer>();
	m_debugRenderer->Initialize();

	//Create System
	std::function<void(Frac::EntityHandle, BoxComponent&, TOR::Transform&)> func
	{
		[this](Frac::EntityHandle e,BoxComponent& box, TOR::Transform& transform)
		{
			if (box.m_enabled)
			{
				glm::vec4 pos = transform.ModelTransformData[3];
				glm::mat4 m = (transform.ModelTransformData);

				const glm::vec4 color = box.color;

				if (transform.Scale.y == 0)
				{
					const glm::vec3 LeftLow = pos - m * (glm::vec4(-0.5f, 0.0f, -0.5f, 0.0f));
					const glm::vec3 LeftHigh = pos - m * (glm::vec4(-0.5f, 0.0f, 0.5f, 0.0f));
					const glm::vec3 RightLow = pos - m * (glm::vec4(0.5f, 0.0f, -0.5f, 0.0f));
					const glm::vec3 RightHigh = pos - m * (glm::vec4(0.5f, 0.0f, 0.5f, 0.0f));

					m_debugRenderer->AddLine(LeftLow, RightLow, color);
					m_debugRenderer->AddLine(RightLow, RightHigh, color);
					m_debugRenderer->AddLine(RightHigh, LeftHigh, color);
					m_debugRenderer->AddLine(LeftHigh, LeftLow, color);
				}
				else
				{
					const glm::vec3 LeftForwardLow = pos - m * (glm::vec4(-0.5, -0.5, -0.5, 0.f));
					const glm::vec3 LeftBackLow = pos - m * (glm::vec4(-0.5, -0.5, 0.5, 0.f));
					const glm::vec3 LeftForwardHigh = pos - m * (glm::vec4(-0.5, 0.5, -0.5, 0.f));
					const glm::vec3 LeftBackHigh = pos - m * (glm::vec4(-0.5, 0.5, 0.5, 0.f));
					const glm::vec3 RightForwardLow = pos - m * (glm::vec4(0.5, -0.5, -0.5, 0.f));
					const glm::vec3 RightBackLow = pos - m * (glm::vec4(0.5, -0.5, 0.5, 0.f));
					const glm::vec3 RightForwardHigh = pos - m * (glm::vec4(0.5, 0.5, -0.5f, 0.f));
					const glm::vec3 RightBackHigh = pos - m * (glm::vec4(0.5, 0.5, 0.5, 0.f));


					m_debugRenderer->AddLine(LeftForwardLow, LeftBackLow, color);
					m_debugRenderer->AddLine(LeftForwardLow, RightForwardLow, color);
					m_debugRenderer->AddLine(LeftForwardLow, LeftForwardHigh, color);
					m_debugRenderer->AddLine(LeftForwardHigh, RightForwardHigh, color);
					m_debugRenderer->AddLine(LeftForwardHigh, LeftBackHigh, color);
					m_debugRenderer->AddLine(LeftBackLow, RightBackLow, color);
					m_debugRenderer->AddLine(LeftBackLow, LeftBackHigh, color);
					m_debugRenderer->AddLine(LeftBackHigh, RightBackHigh, color);
					m_debugRenderer->AddLine(RightBackHigh, RightBackLow, color);
					m_debugRenderer->AddLine(RightBackHigh, RightForwardHigh, color);
					m_debugRenderer->AddLine(RightBackLow, RightForwardLow, color);
					m_debugRenderer->AddLine(RightForwardLow, RightForwardHigh, color);
				}

			}
		}
	};

	m_drawCubeS = m_world->makeSystem<BoxComponent, TOR::Transform>(func, Frac::ePipelineStage::onUpdate, "DrawCube");

}

void GameManager::DrawCursor(const float scale, const glm::vec4& color)
{
	EntityHandle camera = m_cameraManager->GetMainCameraEntity();
	glm::vec3 lookat = m_world->get<CameraManager::CameraTrait>(camera)->LookAt;
	glm::vec3 topLeft{ lookat.x - scale,0.f,lookat.z };
	glm::vec3 topRight{ lookat.x + scale,0.f,lookat.z };
	glm::vec3 bottomLeft{ lookat.x - scale,0.f,lookat.z - scale };
	glm::vec3 bottomRight{ lookat.x + scale,0.f,lookat.z - scale };
	m_debugRenderer->AddLine(topLeft, bottomRight, color);
	m_debugRenderer->AddLine(bottomLeft, topRight, color);
}
#endif

void GameManager::Health_CalculationSystem()
{
	std::function<void(Frac::EntityHandle, HealthComponent&, GridPositionComponent&)> func
	{
		[this](Frac::EntityHandle e,HealthComponent& h, GridPositionComponent& g)
		{
			if (h.m_health <= 0 && m_world->has(e))
			{
				if (m_world->has<EnemyComponent>(e))
				{
					//Add Resource to Player
					m_playerManager->AddResources(m_world->get<EnemyComponent>(e)->m_worth);

					//Free Occupied Tile
					EntityHandle curTile = m_tileManager->m_tileMap->GetTileHandle(g.m_gridPosition);
					m_tileManager->SetTileAvailability(g.m_gridPosition, true, curTile);

					std::vector<EntityHandle> children = m_world->getChilderen(e);
					if (children.size() != 0)
					{
						for (int i = 0; i < children.size(); i++)
						{
							m_world->removeChild(e, children[i]);
							m_world->remove(children[i]);
						}
					}
					//Free Locked Tile (if it exists)
					glm::vec2 enemyNextPos = m_world->get<EnemyComponent>(e)->m_nextGridPos;
					if (enemyNextPos != glm::vec2(-1))
					{
						m_tileManager->SetTileAvailability(m_world->get<EnemyComponent>(e)->m_nextGridPos, true, curTile);
					}
				}
				else if (m_world->has<BaseTag>(e))
				{
					//You lose!
					LOGINFO("Base Destroyed!!");
					m_gameState = GameState::StateDefeat;
				}

				m_world->remove(e);
			}
		}
	};

	m_healthCalculationS = m_world->makeSystem<HealthComponent, GridPositionComponent>(func, Frac::ePipelineStage::postUpdate, "CheckHealth");
}

void GameManager::Turret_FindTargetSystem()
{
	//Get Turrets
	std::function<void(Frac::EntityHandle, TurretComponent&, GridPositionComponent&)> func
	{
		[this](Frac::EntityHandle e,TurretComponent& turret, GridPositionComponent& g)
		{
			if (turret.m_hasTarget == false)
			{
				glm::vec2 result = m_enemyManager->GetEnemyInRange(turret.m_range, g.m_gridPosition);
				if (result == (glm::vec2(-1, -1)))
				{
					turret.m_hasTarget = false;
				}
				else
				{
					turret.m_hasTarget = true;
					turret.m_target = m_tileManager->m_tileMap->GridPosToWorldPos(result);
				}
			}
		}
	};
	m_turretFindTargetS = m_world->makeSystem<TurretComponent, GridPositionComponent>(func, Frac::ePipelineStage::onUpdate, "Turret_FindTargetSystem");
}

void GameManager::Tile_GetLockedSystem()
{
	std::function<void(Frac::EntityHandle, TileComponent&, BoxComponent&, GridPositionComponent&)> func
	{
		[this](Frac::EntityHandle e,TileComponent& tile, BoxComponent& box, GridPositionComponent& g)
		{
			if (tile.m_states.m_isLocked)
			{
				box.color = { 1, 0.5, 0, 1 };
			}
			/*if (m_tileManager->IsTileOccupied(g.m_gridPosition))
			{
				box.color = { 1, 0.5, 0, 1 };
			}*/
			else
			{
				box.color = { 1, 1, 1, 1 };
			}
		}
	};
	m_tileGetLockedS = m_world->makeSystem<TileComponent, BoxComponent, GridPositionComponent>(func, Frac::ePipelineStage::onUpdate, "Tile_GetLockedSystem");
}

void GameManager::OnGameStateChange()
{
	if (m_lastGameState == m_gameState)
	{
		return;
	}

	m_lastGameState = m_gameState;

	switch (m_gameState)
	{
	case GameState::StateMainMenu:
	{
		// use demo camera
		if (m_cameraManager->GetMainCameraEntity() != m_camDemo)
		{
			m_cameraManager->SetMainCameraEntity(m_camDemo);
		}

		ManageSystems(false);
		m_cameraManager->m_updateMainCameraS.enable();
	}
	break;
	case GameState::StatePauseMenu:
	{
		ManageSystems(false);
		m_isPaused = true;
	}
	break;
	case GameState::StatePlaying:
	{
		// switch to game camera
		if (m_cameraManager->GetMainCameraEntity() != m_camGame)
		{
			m_cameraManager->SetMainCameraEntity(m_camGame);
		}

		ManageSystems(true);
		m_isPaused = false;

	}
	break;
	case GameState::StateVictory:
	{
		// use demo camera
		if (m_cameraManager->GetMainCameraEntity() != m_camDemo)
		{
			m_cameraManager->SetMainCameraEntity(m_camDemo);
		}
		ManageSystems(false);
		m_cameraManager->m_updateMainCameraS.enable();

		// show victory/defeat canvas

	}
	break;
	case GameState::StateDefeat:
	{
		// use demo camera
		if (m_cameraManager->GetMainCameraEntity() != m_camDemo)
		{
			m_cameraManager->SetMainCameraEntity(m_camDemo);
		}
		ManageSystems(false);
		m_cameraManager->m_updateMainCameraS.enable();

		// show defeat canvas

	}
	break;
	case GameState::StateQuitGame:
	{
		// TODO Quit game 
		m_connector->GetEngine().SetRunState(false);
	}
	break;
	}
}

void GameManager::Turret_ShootTargetSystem()
{
	std::function<void(Frac::EntityHandle, TurretComponent&, DirectionComponent&, TOR::Transform&, GridPositionComponent&)> func
	{
		[this](Frac::EntityHandle e,TurretComponent& turret, DirectionComponent& d, TOR::Transform& t, GridPositionComponent& g)
		{
			turret.m_timeElapsed += m_world->getDeltaTime();

			if ((turret.m_hasTarget) && (turret.m_timeElapsed >= turret.m_firerate))
			{
				m_bulletCounter++;
				turret.m_timeElapsed = 0.0f;
				d.m_direction = glm::normalize(turret.m_target - t.Position);
				turret.m_hasTarget = false;
				float bulletSize = m_tileManager->m_tileMap->m_size / 15.f;
				//Create Bullet Entity
				m_world->makeEntity<BoxComponent, GridPositionComponent, DirectionComponent, VelocityComponent, TOR::Transform, BulletComponent>
					("Bullet " + std::to_string(m_bulletCounter),
						{ {glm::vec4(0,1,0,1)}, true },
						{ {g.m_gridPosition} },
						{ {d.m_direction} },
						{ {turret.m_bulletSpeed} },
						{ {t.Position}, {glm::vec3(bulletSize)} },
						{ {} });
			}
		}
	};
	m_turretShootTargetS = m_world->makeSystem<TurretComponent, DirectionComponent, TOR::Transform, GridPositionComponent>(func, Frac::ePipelineStage::onUpdate, "Turret_ShootTargetSystem");
}

void GameManager::Bullet_MovementSystem()
{
	std::function<void(Frac::EntityHandle, BulletComponent&, DirectionComponent&, TOR::Transform&, VelocityComponent&, GridPositionComponent&)> func
	{
		[this](Frac::EntityHandle e,BulletComponent& bullet, DirectionComponent& d, TOR::Transform& t, VelocityComponent& v, GridPositionComponent& g)
		{
			t.Position += (d.m_direction * (v.m_speed * m_world->getDeltaTime()));

			g.m_gridPosition = m_tileManager->m_tileMap->WorldPosToGridPos(t.Position);

			bool isTileOccupied = m_tileManager->IsTileOccupied(g.m_gridPosition);

			EntityHandle curTile = m_tileManager->m_tileMap->GetTileHandle(g.m_gridPosition);
			const TileComponent* curTileComp = m_world->get<TileComponent>(curTile);

			if ((isTileOccupied) && m_world->has(curTileComp->m_occupier))
			{
				if (m_world->has<EnemyComponent>(curTileComp->m_occupier))
				{
					int& health = m_world->get<HealthComponent>(curTileComp->m_occupier)->m_health;
					health -= bullet.m_damage;
					m_world->remove(e);
				}
			}
			//Check if bullet hit any walls
			else if ((curTileComp->m_states.m_isActive == false) || ((curTileComp->m_states.m_isWalkable == false) && (curTileComp->m_states.m_isBuildable == true)))
			{
				m_world->remove(e);
			}
		}
	};
	m_bulletMovementS = m_world->makeSystem<BulletComponent, DirectionComponent, TOR::Transform, VelocityComponent, GridPositionComponent>(func, Frac::ePipelineStage::onUpdate, "Bullet_MovementSystem");
}
