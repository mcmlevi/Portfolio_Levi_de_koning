#pragma once
#include "Graphics/DebugRenderer.h"
#include "TileMap.h"
#include "Core/ECSWrapper.h"
#include "Core/EngineConnector.h"
#include <unordered_set>

class TileManager
{
public:

	TileManager(Frac::EngineConnector* connector);

	~TileManager();

	void CreateTileMap(uint rows, uint columns, std::vector<std::vector<int>>* gridData);

	void RunFloodFillAlgorithm();
	void ResetFloodFillCost();

	void RunAStarALgorithm(glm::vec2 gridStartPosition, glm::vec2 gridTargetPosition);
	void RetracePath(EntityHandle startEH, EntityHandle targetEH);
	std::vector<EntityHandle> m_LastRetracedPath;
	void SetTileOccupancy(glm::vec2 gridPos, bool occupy, EntityHandle e);
	void SetTileLocked(glm::vec2 gridPos, bool lock);
	void SetTileAvailability(glm::vec2 gridPos, bool isAvailable, EntityHandle e);
	bool IsTileOccupied(glm::vec2 gridPos);
	bool IsTileLocked(glm::vec2 gridPos);
	bool CanMoveIntoTile(glm::vec2 gridPos);

	glm::vec2 GetNextTilePos(size_t tileID, uint patience);
	const size_t GetTileID(const glm::vec2 enemyGridPos);

	bool DidGridFill();
	bool IsInGrid(glm::vec2 gridPos);
	void SetTileNeighbors();
	int GetDistance(EntityHandle tileA, EntityHandle tileB);
	
	std::unique_ptr<TileMap> m_tileMap;

private:
	Frac::EngineConnector* m_connector;

	Frac::Query<BaseTag, GridPositionComponent> m_runffq;
	Frac::Query<TileComponent> m_tileq;
	Frac::Query<GridPositionComponent, TileComponent> m_gridTileq;
};



