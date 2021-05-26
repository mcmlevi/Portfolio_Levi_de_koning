#include "pch.h"
#include "LevelManager.h"
#include "Core/EngineCore.h"
#include "fstream"

LevelManager::LevelManager(Frac::EngineConnector* connector, TileManager* a_tileManager, LevelEntityCreator* levelEntityCreator, EnemyEntityCreator* enemyEntityCreator, PlayerEntityCreator* playerEntityCreator) : m_connector(connector)
{
	m_ogmoParser = std::make_unique<OgmoParser>(connector);
	m_tileManager = a_tileManager;
	m_levelEntityCreator = levelEntityCreator;
	m_enemyEntityCreator = enemyEntityCreator;
	m_playerEntityCreator = playerEntityCreator;
}

LevelManager::~LevelManager()
{
	m_connector = nullptr;
}

void LevelManager::Init()
{
	PreLoadLevelData();
}

void LevelManager::PreLoadLevelData()
{
	//Clearing old pre loaded level data
	m_preLoadedLevelData.clear();

	const std::string JsonString = m_connector->GetEngine().GetResourceManager().GetFileIO().Read("[AllLevelData]LevelData.json");
	m_activePreloadedLevelDataFile = json::parse(JsonString);
	m_preLoadedLevelsAmount = m_activePreloadedLevelDataFile["levelData"].size();
	LOGINFO("Amount of pre loaded levels: %i", m_preLoadedLevelsAmount);

	
	for (unsigned int i = 0; i < m_activePreloadedLevelDataFile["levelData"].size(); i++) {
		std::shared_ptr<LevelData> newLevelData = std::make_shared<LevelData>();
		newLevelData->m_levelId = m_activePreloadedLevelDataFile["levelData"][i]["m_levelId"];
		newLevelData->m_levelName = m_activePreloadedLevelDataFile["levelData"][i]["m_levelName"];
		newLevelData->m_levelTargetScore = m_activePreloadedLevelDataFile["levelData"][i]["m_levelTargetScore"];
		newLevelData->m_levelWaves = m_activePreloadedLevelDataFile["levelData"][i]["m_levelWaves"];
		newLevelData->m_cameraStartPosition = glm::vec3(m_activePreloadedLevelDataFile["levelData"][i]["camStartPosition"]["x"], m_activePreloadedLevelDataFile["levelData"][i]["camStartPosition"]["y"], m_activePreloadedLevelDataFile["levelData"][i]["camStartPosition"]["z"]);
		newLevelData->m_cameraStartRotation = glm::vec3(m_activePreloadedLevelDataFile["levelData"][i]["camStartrotation"]["x"], m_activePreloadedLevelDataFile["levelData"][i]["camStartrotation"]["y"], m_activePreloadedLevelDataFile["levelData"][i]["camStartrotation"]["z"]);
		m_preLoadedLevelData.push_back(newLevelData);
	}
}

void LevelManager::LoadLevel(int level)
{
	if (level > m_preLoadedLevelsAmount -1)
	{
		LOGWARNING("Tryingg to load a undefined level, no level is loaded");
		return;
	}

	m_activeLevelData = m_preLoadedLevelData[level];
	//Getting the right data out of the actual level json file using the ogmo parser.
	//setting the right selected level data in the ogmo parser
	m_ogmoParser->SetActiveLevelFile(m_activeLevelData->m_levelName);
	//Loading grid data from the selected level
	m_ogmoParser->ParseOgmoGridData();
	//Loading all the entity data from the selected level
	m_ogmoParser->ParseStaticLevelInteractables();

	//Actual data getting and setting
	m_activeLevel = level;

	//Clearing older data
	m_activeLevelGridData.clear();
	m_allActiveStaticCores.clear();
	m_allActiveStaticEnemySpawners.clear();
	m_allActiveStaticTowers.clear();

	//Getting and setting the new data from ogmoparser parsed data
	m_activeLevelGridData = m_ogmoParser->m_ParsedLevel;
	m_tileMapSize = m_ogmoParser->m_gridSize;
	m_ogmoSpriteSize = m_ogmoParser->m_spriteSize;
	m_allActiveStaticCores = m_ogmoParser->m_allActiveStaticCores;
	m_allActiveStaticEnemySpawners = m_ogmoParser->m_allActiveStaticEnemySpawners;
	m_allActiveStaticTowers = m_ogmoParser->m_allActiveStaticTowers;

	//TODO add change camera position and rotation command
	SetGameTileManagerData();
	m_levelEntityCreator->CreateStaticLevelModel(level);
	CreateAllPreLoadedLevelEntities();
}

void LevelManager::PrintAllPreLoadedLevelData()
{
	for (unsigned int i = 0; i < m_preLoadedLevelData.size(); i++)
	{
		std::cout << m_preLoadedLevelData[i]->m_levelId << std::endl;
		std::cout << m_preLoadedLevelData[i]->m_levelName << std::endl;
		std::cout << m_preLoadedLevelData[i]->m_levelTargetScore << std::endl;
		std::cout << m_preLoadedLevelData[i]->m_levelWaves << std::endl;
		std::cout << m_preLoadedLevelData[i]->m_cameraStartPosition.x << " " << m_preLoadedLevelData[i]->m_cameraStartPosition.y << " " << m_preLoadedLevelData[i]->m_cameraStartPosition.z << std::endl;
		std::cout << m_preLoadedLevelData[i]->m_cameraStartRotation.x << " " << m_preLoadedLevelData[i]->m_cameraStartRotation.y << " " << m_preLoadedLevelData[i]->m_cameraStartRotation.z << std::endl;
		std::cout << std::endl;
	}
}

void LevelManager::PrintOgmoParsedLevelData()
{
	for (unsigned int i = 0; i < m_activeLevelGridData.size(); i++)
	{
		for (unsigned int y = 0; y < m_activeLevelGridData[i].size(); y++)
		{
			std::cout << m_activeLevelGridData[i][y];
		}
		std::cout << std::endl;
	}
}

void LevelManager::SetGameTileManagerData()
{
	//This will be used to set the tileManager data to the ogmo data
	m_tileManager->CreateTileMap(m_tileMapSize.x, m_tileMapSize.y, &m_activeLevelGridData);
}

void LevelManager::CreateAllPreLoadedLevelEntities()
{
	//This will be used to create and add all the actual antities using the resource managers
	for (unsigned int i = 0; i < m_allActiveStaticEnemySpawners.size(); i++)
	{
		m_enemyEntityCreator->CreateSpawner(m_allActiveStaticEnemySpawners[i].m_objectId,
		m_allActiveStaticEnemySpawners[i].m_position /= m_ogmoSpriteSize,
		m_allActiveStaticEnemySpawners[i].m_rotation,
		m_allActiveStaticEnemySpawners[i].m_enemySpawnType,
		m_allActiveStaticEnemySpawners[i].m_enemySpawnDelay);
	}
	for (unsigned int i = 0; i < m_allActiveStaticCores.size(); i++)
	{
		m_playerEntityCreator->CreatePlayerBase(m_allActiveStaticCores[i].m_objectId,
		(m_allActiveStaticCores[i].m_position /= m_ogmoSpriteSize),
		glm::vec3(0,0,0), m_allActiveStaticCores[i].m_coreHealth,
		m_allActiveStaticCores[i].m_coreTargetData);
	}
	for (unsigned int i = 0; i < m_allActiveStaticTowers.size(); i++)
	{
		glm::vec2 selectedTile = glm::vec2(m_allActiveStaticTowers[i].m_position.x /= m_ogmoSpriteSize, m_allActiveStaticTowers[i].m_position.z /= m_ogmoSpriteSize);
		glm::vec3 selectedTileWorldSpace = glm::vec3(selectedTile.x, 0, selectedTile.y);
		m_playerEntityCreator->CreateTower(0, selectedTileWorldSpace, glm::vec3(0,0,0), 0, 0);
		size_t currentTileID = m_tileManager->GetTileID(selectedTile);
		TileComponent* curTile = m_connector->GetEngine().GetWorld().get<TileComponent>(m_tileManager->m_tileMap->m_tileEntities->at(currentTileID));
		curTile->m_states.m_isBuildable = false;
	}
}


