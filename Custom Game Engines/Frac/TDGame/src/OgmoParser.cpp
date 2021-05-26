#include "pch.h"
#include "OgmoParser.h"

OgmoParser::OgmoParser(Frac::EngineConnector* connector) : m_connector(connector)
{
	
}

OgmoParser::~OgmoParser()
{

}

void OgmoParser::SetActiveLevelFile(std::string filepath)
{
	const std::string JsonString = m_connector->GetEngine().GetResourceManager().GetFileIO().Read("[Levels]" + filepath);
	m_ActiveJsonFile = json::parse(JsonString);
}

void OgmoParser::ParseOgmoGridData() 
{
	m_gridSize = glm::vec2(m_ActiveJsonFile["gridCellsX"], m_ActiveJsonFile["gridCellsY"]);
	m_spriteSize = m_ActiveJsonFile["gridCellWidth"];
	LOGINFO("Grid size x:  %f", m_gridSize.x);
	LOGINFO("Grid size y:  %f", m_gridSize.y);
	m_ParsedLevel = m_ActiveJsonFile["Level"][0]["LevelData"].get<std::vector<std::vector<int>>>();
}

void OgmoParser::ParseStaticLevelInteractables()
{
	m_allActiveStaticEnemySpawners.clear();
	m_allActiveStaticCores.clear();
	m_allActiveStaticTowers.clear();

	//Handling the core entities data
	for (unsigned int i = 0; i < m_ActiveJsonFile["CoreEntities"][0]["entities"].size(); i++)
	{
		LOGINFO("parsed a static core entity");
		StaticLevelCore staticLevelCore;
		staticLevelCore.m_position = glm::vec3(m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["x"],0 , m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["y"]); //TODO check what the height will be //TODO make sure that the used result is divided by grid height and width.
		staticLevelCore.m_name = m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["name"];
		staticLevelCore.m_objectId = m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["values"]["ObjectID"];
		staticLevelCore.m_coreHealth = m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["values"]["CoreHealth"];
		staticLevelCore.m_coreTargetData = m_ActiveJsonFile["CoreEntities"][0]["entities"][i]["values"]["CoreTargetData"];
		m_allActiveStaticCores.push_back(staticLevelCore);
	}

	//Handling the enemy spawner entities data
	for (unsigned int i = 0; i < m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"].size(); i++)
	{
		LOGINFO("parsed a static enemy spawner entity");
		StaticLevelEnemySpawner staticLevelEnemySpawner;
		staticLevelEnemySpawner.m_position = glm::vec3(m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["x"], 0, m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["y"]);
		staticLevelEnemySpawner.m_name = m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["name"];
		staticLevelEnemySpawner.m_objectId = m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["values"]["ObjectID"];
		staticLevelEnemySpawner.m_enemySpawnType = m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["values"]["EnemySpawnType"];
		staticLevelEnemySpawner.m_enemySpawnDelay = m_ActiveJsonFile["EnemySpawnerEntities"][0]["entities"][i]["values"]["EnemySpawnDelay"];
		m_allActiveStaticEnemySpawners.push_back(staticLevelEnemySpawner);
	}

	//Handling the static tower entities data
	for (unsigned int i = 0; i < m_ActiveJsonFile["TowerEntities"][0]["entities"].size(); i++)
	{
		LOGINFO("parsed a static Tower entity");
		StaticLevelTower staticLevelTower;
		staticLevelTower.m_position = glm::vec3(m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["x"], 0, m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["y"]);
		staticLevelTower.m_name = m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["name"];
		staticLevelTower.m_objectId = m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["values"]["ObjectID"];
		staticLevelTower.m_bulletType = m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["values"]["BulletType"];
		staticLevelTower.m_towerHealth = m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["values"]["TowerHealth"];
		staticLevelTower.m_fireRate = m_ActiveJsonFile["TowerEntities"][0]["entities"][i]["values"]["FireRate"];
		m_allActiveStaticTowers.push_back(staticLevelTower);
	}
}
