#pragma once
#include "Core/EngineCore.h"
#include "Nlohmann/json.hpp"
#include "StaticLevelInteractable.h"
#include "StaticLevelEnemySpawner.h"
#include "StaticLevelCore.h"
#include "StaticLevelTower.h"
using json = nlohmann::json;
class OgmoParser {
public:
	OgmoParser(Frac::EngineConnector* connector);
	~OgmoParser();
	void SetActiveLevelFile(std::string filepath);
	void ParseOgmoGridData();
	void ParseStaticLevelInteractables();
	
	std::vector<std::vector<int>> m_ParsedLevel;

	std::vector<StaticLevelEnemySpawner>m_allActiveStaticEnemySpawners;
	std::vector<StaticLevelCore>m_allActiveStaticCores;
	std::vector<StaticLevelTower>m_allActiveStaticTowers;
	int m_spriteSize;
	glm::vec2 m_gridSize;
private:
	Frac::EngineConnector* m_connector;
	json m_ActiveJsonFile;
};
