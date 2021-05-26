#pragma once
#include "Components.h"
#include "Logger.h"
#include "Core/ECSWrapper.h"
#include "TileManager.h"
#include "EnemyManager.h"
#include "LevelManager.h"
#include "PlayerManager.h"
#include "CameraManager.h"
#include "LevelEntityCreator.h"
#include "EnemyEntityCreator.h"
#include "PlayerEntityCreator.h"
#include "UIManager.h"
#include "EGameState.h"

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
#include "Graphics/DebugRenderer.h"
#endif

class GameManager
{
public:

	GameManager(Frac::EngineConnector* connector, Frac::ECSWrapper* world);

	~GameManager();

	//Public Functions
	void ManageSystems(bool activateSystem);
	void UpdateGame();
	glm::vec2 GetSelectedGridPosition();

	//Systems
	Frac::System<HealthComponent, GridPositionComponent> m_healthCalculationS;
	Frac::System<TurretComponent, GridPositionComponent> m_turretFindTargetS;
	Frac::System<TurretComponent, DirectionComponent, TOR::Transform, GridPositionComponent> m_turretShootTargetS;
	Frac::System<BulletComponent, DirectionComponent, TOR::Transform, VelocityComponent, GridPositionComponent> m_bulletMovementS;
	Frac::System<TileComponent, BoxComponent, GridPositionComponent> m_tileGetLockedS;
	Frac::System<BoxComponent, TOR::Transform> m_drawCubeS;

	//Managers
	TileManager* m_tileManager;
	Frac::SceneManager* m_sceneManager;
	std::unique_ptr<EnemyManager> m_enemyManager;
	std::unique_ptr<LevelManager> m_levelManager;
	std::unique_ptr<CameraManager> m_cameraManager;
	std::unique_ptr<PlayerManager> m_playerManager;
	std::unique_ptr<LevelEntityCreator> m_levelEntityCreator;
	std::unique_ptr<EnemyEntityCreator> m_enemyEntityCreator;
	std::unique_ptr<PlayerEntityCreator> m_playerEntityCreator;
	std::unique_ptr<UIManager> m_uiManager;
	
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	std::unique_ptr<Frac::DebugRenderer> m_debugRenderer;
	glm::mat4 m_viewProj;

	void InitDebugRenderer();
	void DrawCursor(const float scale = 1.0f, const glm::vec4& color = { 1.f,0.f,0.f,0.f });
#endif

private:
	//Private Functions
	void Init();
	void Health_CalculationSystem();
	void Turret_FindTargetSystem();
	void Turret_ShootTargetSystem();
	void Bullet_MovementSystem();
	void Tile_GetLockedSystem();

	void OnGameStateChange();
	
	uint m_bulletCounter;
	bool m_isPaused;

	Frac::EngineConnector* m_connector;
	Frac::ECSWrapper* m_world;
	Frac::InputSystem* m_inputSystem;

	GameState m_gameState;
	GameState m_lastGameState;

	Frac::EntityHandle m_camGame; /*! <- camera used for game play */
	Frac::EntityHandle m_camDemo; /*! <- camera used for demo (main-menu preview, etc) */
};

