/*
 * \class TileMap
 *
 * \brief Responsible for passing the data of a Tile when needed.
 *
 * \author Antreas
 * \date March 2021
 */

#pragma once
#include "Components.h"
#include "Core/EngineConnector.h"
#include "CoreRenderAPI/Components/Transform.h"

class TileMap
{
public:
	/*
	* \brief TileMap Constructor
	* \width The 'X' Dimensions of our Grid
	* \lenght The 'Y' Dimensions of our Grid
	* \ogmoData The Ogmo data after they have been processed by the Level Manager
	*/
	TileMap(uint x, uint y, Frac::EngineConnector* connector);
	~TileMap();

	void CreateTiles(std::vector<std::vector<int>>* gridData);
	void PrintTileInfo();
	void PrintGrid();

	glm::vec3 GridPosToWorldPos(glm::vec2 gridPos);
	glm::vec2 WorldPosToGridPos(glm::vec3 worldPos);

	const EntityHandle GetTileHandle(glm::vec2 gridPos);
	const TileComponent* GetTileComponent(const EntityHandle e);
	const TOR::Transform* GetTileTransformComponent(const EntityHandle e);
	const GridPositionComponent* GetTileGridPosComponent(const EntityHandle e);
	const BoxComponent* GetTileBoxComponent(const EntityHandle e);
	
	std::unique_ptr<std::vector<EntityHandle>> m_tileEntities;
	const uint m_rows;
	const uint m_cols;
	const float m_size;
private:	
	Frac::EngineConnector* m_connector;
};

