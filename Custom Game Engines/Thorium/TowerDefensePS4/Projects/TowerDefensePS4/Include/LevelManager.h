#pragma once
#include "pch.h"
#include <Nlohmann/Include/json.hpp>
#include <iostream>
#include <Graphics/RenderCore.h>
#include "TileMap.h"
using json = nlohmann::json;
class LevelManager {
public:
	LevelManager();
	~LevelManager();

	void SetTargetJsonFile(json& file) {
		m_ActiveParsedLevel = file;
	}
	void LoadLevel(std::string level);
	void PlaceMainTiles();
	void PlaceConnectiveTiles();
	void SetAllNeighbouringTiles();
	void SetconnectiveTileModels();
	void SetAllTileInteractAbles();
	void PlaceEnemyBeginEndPoint();
	void SetCamera(entt::entity& newCamera);
	void CreateLevelArrow();
	entt::entity m_Camera;
	void CalculateFullPath();
	void CalculatePreferredPath();
	void SetDistancePenalty();
	void SetTileModelShader(Th::RenderCore* renderCore);
	void OnMouseDown(Th::MouseButtonEventArgs);
	void SetEmptySideTilesInteractAble(Tile* checkTile);
	json m_ActiveParsedLevel;
	TileMap m_AllTiles2D;
	float CalculateArrowOffset(int TileType);
	ModelComponent* GetTileRelatedModel(int interactableType, int tileType);
	entt::entity m_TileMapEntity{};
	std::vector<Tile*>m_GeneratedPath;
	std::vector<Tile*>m_PreferredPath;
	//PathFindingComponent* m_PathFindingComponent{};

	entt::entity m_LevelArrow;
	Tile* m_EnemyBeginTile = nullptr;
	Tile* m_EnemyTargetTile = nullptr;
	Tile* m_SelectedTile = nullptr;
private:
	void GetAllNeighboringTiles(Tile* checkpoint);
	bool InField(int checkValue, int min, int max);

	glm::vec2 m_MapDimensions{ glm::vec2(60, 60) };
	bool m_LevelIsLoading = false;

	enum Tiles {
		MainTile,
		DiagonalTileUp,
		CornerTileUp,
		CornerTileDown,
	};

	enum AllSides {
		LeftUp,
		Up,
		RightUp,
		Right,
		RightDown,
		Down,
		LeftDown,
		Left
	};

	glm::vec2 m_Sides[8] = {
		glm::vec2(-1,1),
		glm::vec2(0,1),
		glm::vec2(1,1),
		glm::vec2(1,0),
		glm::vec2(1,-1),
		glm::vec2(0,-1),
		glm::vec2(-1,-1),
		glm::vec2(-1,0)
	};

	ModelComponent* m_AllInteractables[12][4];
	ModelComponent* m_AllTowerModels[2];
	ModelComponent* m_CheckPointModel;
	ModelComponent* m_AllTileModels[4]{
	nullptr,
	nullptr,
	nullptr,
	nullptr
	};
	ModelComponent* m_ArrowModel;

	glm::vec2 m_EnemyBeginPosition;
	glm::vec2 m_EnemyTargetPosition;



	bool m_EnemyBeginPositionSet = false;
	bool m_EnemyTargetPositionSet = false;
};
