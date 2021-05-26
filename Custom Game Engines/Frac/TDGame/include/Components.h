#pragma once
#include "Glm/glm.hpp"
#include "Core/ECSWrapper.h"
#include "SceneManagement/Components/TransformComponent.h"
#include <vector>

typedef unsigned int uint;

enum EEnemyType
{
	None = 0,
	Red,
	Green,
	Blue
};

/// These are Empty Structs
/// Tags
/// Based on Flecs Documentation, these can be used as Tags
struct EnemyTag {};
struct PlayerTag {};
struct BaseTag {};


/// These are all of the Components used by Gameplay
/// Components
/// Components simply have Data and no functionality
struct VelocityComponent
{
	float m_speed;
};

struct GridPositionComponent
{
	glm::vec2 m_gridPosition;
};

struct DirectionComponent
{
	glm::vec3 m_direction;
};

struct BoxComponent 
{
	glm::vec4 color = {1.f, 1.f, 1.f, 1.f};
	bool m_enabled = true;
};

struct TileStatesComponent
{
	bool m_isActive = true;
	bool m_isWalkable = true;
	bool m_isBuildable = true;
	bool m_isClickable = true;
	bool m_didCheck = false;
	bool m_isLocked = false;
};

struct FloodFillComponent
{
	uint m_floodFillCost = 9999;
};

struct AStarComponent
{
	int m_gCost =0;
	int m_hCost =0;
	std::vector<EntityHandle>* m_tileAStarUsers = nullptr; //holds Entities that use that Tile for A* Star.
	std::vector<EntityHandle> m_SideTiles;
	int m_heapIndex = -1;
	EntityHandle m_parentEH = 0;
	int GetFCost() {
		return m_gCost + m_hCost;
	}
	
};

struct TileComponent
{
	size_t m_ID = 9999;
	EntityHandle m_occupier = (uint64_t)0;
	TileStatesComponent m_states;
	FloodFillComponent m_floodFill;
	AStarComponent m_aStar;
};

struct EnemyComponent
{
	EEnemyType m_enemyType = EEnemyType::Red;
	int m_patience = 1; //max value
	float m_patienceTimer = 3.0f;
	float m_timeElapsed = 0.0f;
	uint m_worth = 50;
	uint m_damageToBase = 5;
	glm::vec3 m_target = { 0,0,0 };
	glm::vec2 m_nextGridPos = { -1, -1 };
	bool m_hasTarget = false;
	bool m_didReachTile = false;
};

struct HealthComponent
{
	int m_health = 100;
};

struct TurretComponent
{
	bool m_hasTarget = false;
	glm::vec3 m_target = { 0,0,0 };
	uint m_range = 3;
	float m_firerate = 1.f;
	float m_timeElapsed = 0.0f;
	float m_bulletSpeed = 30.0f;
};

struct BulletComponent
{
	bool m_isMissile = false;
	uint m_damage = 100;
};

struct SpawnerComponent
{
	EEnemyType m_spawnerType = EEnemyType::None;	
	float m_spawnrate = 2.0f;
	float m_timeElapsed = 0.0f;	
};