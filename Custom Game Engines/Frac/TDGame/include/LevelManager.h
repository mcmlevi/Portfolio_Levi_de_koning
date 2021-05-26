#pragma once
#include <vector>
#include "Glm/gtx/transform.hpp"
#include "OgmoParser.h"
#include "LevelData.h"
#include "StaticLevelInteractable.h"
#include "StaticLevelEnemySpawner.h"
#include "StaticLevelCore.h"
#include "StaticLevelTower.h"
#include "TileManager.h"
#include "LevelEntityCreator.h"
#include "EnemyEntityCreator.h"
#include "PlayerEntityCreator.h"
namespace Frac{
	class EngineCore;
}

class LevelManager {
public:
	LevelManager(Frac::EngineConnector* connector, TileManager* a_tileManager, LevelEntityCreator* levelEntityCreator, EnemyEntityCreator* enemyEntityCreator, PlayerEntityCreator* playerEntityCreator);
	~LevelManager();
	void Init();
	void PreLoadLevelData();
	void LoadLevel(int level);
	
	int m_activeLevel;
	std::vector<std::vector<int>> m_activeLevelGridData;
	glm::vec2 m_tileMapSize;
	std::shared_ptr<LevelData> m_activeLevelData;
	int m_ogmoSpriteSize;

private:
	//Make shared pointer
	Frac::EngineConnector* m_connector;

    //Make unique pointer
	std::unique_ptr<OgmoParser> m_ogmoParser;

	//Make shared pointer
	TileManager* m_tileManager;
	//std::shared_ptr<TileManager> m_tileManager;

	LevelEntityCreator* m_levelEntityCreator;
	EnemyEntityCreator* m_enemyEntityCreator;
	PlayerEntityCreator* m_playerEntityCreator;
	
	std::vector<std::shared_ptr<LevelData>> m_preLoadedLevelData;
	
	std::vector<StaticLevelEnemySpawner>m_allActiveStaticEnemySpawners;
	std::vector<StaticLevelCore>m_allActiveStaticCores;
	std::vector<StaticLevelTower>m_allActiveStaticTowers;

	json m_activePreloadedLevelDataFile;
	void PrintAllPreLoadedLevelData();
	void PrintOgmoParsedLevelData();
	void SetGameTileManagerData();
	void CreateAllPreLoadedLevelEntities();

	int m_preLoadedLevelsAmount;
};
