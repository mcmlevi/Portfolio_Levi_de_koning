#include "pch.h"
#include "TileMap.h"
#include "Core/EngineCore.h"
#include <iomanip>

TileMap::TileMap(uint x, uint y, Frac::EngineConnector* connector) : m_rows(x), m_cols(y), m_connector(connector), m_size(1.0f)
{
	m_tileEntities = std::make_unique<std::vector<EntityHandle>>();
	m_tileEntities->resize(x * y); //allocating space for all tiles
	LOGINFO("Tile Map Initialized");
}

TileMap::~TileMap()
{
	m_connector = nullptr;
	delete m_connector;
}

void TileMap::CreateTiles(std::vector<std::vector<int>>* gridData)
{
	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();

	EntityHandle prefab = world.makePrefab<GridPositionComponent, TOR::Transform, BoxComponent, TileComponent>
		({ "Tile_Prefab" }
	, {}
	, {}
	, {}
	, {});

	EntityHandle prefab_2 = world.makePrefab<GridPositionComponent, BoxComponent, TileComponent>
		({ "E_Prefab" }
			, {}
			, {}
			, {});
	
	for (uint x = 0; x < m_rows; x++)
	{
		for (uint y = 0; y < m_cols; y++)
		{
			bool isActive = true;
			bool isBuildAble = false;
			if ((*gridData)[y][x] < 1) 
			{
				isActive = false;
			} 

			if ((*gridData)[y][x] == 3)
			{
				isBuildAble = true;
			}

			size_t currentTileID = (size_t)(y + x * m_cols);
			EntityHandle& entity = (*m_tileEntities)[currentTileID];
			

			if (isActive == true)
			{
				
				entity = world.makeFromPrefab(prefab, { "Tile" + std::to_string(currentTileID) });

				world.set<GridPositionComponent>(entity, { glm::vec2(x, y) });
				world.set<TOR::Transform>(entity, { {GridPosToWorldPos(glm::vec2(x, y))}, {glm::vec3(m_size, 0.0f, m_size)}, {}, {}, {}, {true}, {true} });
				world.set<BoxComponent>(entity, { {glm::vec4(1.f, 1.f, 1.f, 1.f)}, true });
				world.set<TileComponent>(entity, { currentTileID });

				if (isBuildAble)
				{
					world.get<TileComponent>(entity)->m_states.m_isWalkable = false;
					world.get<BoxComponent>(entity)->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); 
				}
			}
			 
			else //This is used to avoid showing the inactive Tiles. We can add a Tool later on that has options to choose from.
			{
				entity = world.makeFromPrefab(prefab_2, { "Tile" + std::to_string(currentTileID) });

				world.set<GridPositionComponent>(entity, { glm::vec2(x, y) });
				//world.set<TOR::Transform>(entity, { {GridPosToWorldPos(glm::vec2(x, y))}, {glm::vec3(m_size, 0.0f, m_size)}, {}, {}, {}, {false}, {true} });
				world.set<BoxComponent>(entity, { {glm::vec4(1.f, 1.f, 1.f, 1.f)}, false });
				world.set<TileComponent>(entity, { currentTileID });
				world.get<TileComponent>(entity)->m_states = { false, false, false, false, false };
			}
			
		}
	}
}

void TileMap::PrintTileInfo()
{
	
	std::cout << "All Tiles are: " << m_tileEntities->size() << std::endl; //check how many Tiles we have

	for (uint i = 0; i < m_rows; i++)
	{
		for (uint k = 0; k < m_cols; k++)
		{
			size_t currentTileID = (size_t)(k + i * m_cols);
			EntityHandle e = m_tileEntities->at(currentTileID);
			std::cout << "Grid X: " << GetTileGridPosComponent(e)->m_gridPosition.x << " Grid Y: " << GetTileGridPosComponent(e)->m_gridPosition.y << std::endl;
			std::cout << "Pos X: " << GetTileTransformComponent(e)->Position.x << " Pos Y: " << GetTileTransformComponent(e)->Position.y << " Pos Z: " << GetTileTransformComponent(e)->Position.z << std::endl;
			std::cout << "FloodFillCost: " << GetTileComponent(e)->m_floodFill.m_floodFillCost << std::endl;
		}
	}

}

void TileMap::PrintGrid()
{
	std::cout << "Flood Fill: " << std::endl;

	for (uint i = 0; i < m_rows; i++)
	{
		for (uint k = 0; k < m_cols; k++)
		{
			size_t currentTileID = (size_t)(k + i * m_cols);
			EntityHandle e = m_tileEntities->at(currentTileID);
			std::cout << std::setw(5) << GetTileComponent(e)->m_floodFill.m_floodFillCost;
		}
		std::cout<< std::endl;
	}
}

glm::vec3 TileMap::GridPosToWorldPos(glm::vec2 gridPos)
{
	return glm::vec3(((gridPos.y * m_size) + (m_size/2.0f)), 0.0f, -((gridPos.x * m_size) + (m_size / 2.0f))); //center of Tile
}

glm::vec2 TileMap::WorldPosToGridPos(glm::vec3 worldPos)
{
	glm::vec2 gridPos = glm::round(glm::vec2(-(worldPos.z / m_size), (worldPos.x / m_size)));

	if ((gridPos.x < 0) || (gridPos.x >= m_rows) || (gridPos.y < 0) || (gridPos.y >= m_cols))
	{
		//LOGERROR("WorldPosToGridPos Function. Invalid Coordinates %f, %f, %f", worldPos.x, worldPos.y, worldPos.z);
		return glm::vec2(-1);
	}

	return glm::round(gridPos);
}

const EntityHandle TileMap::GetTileHandle(glm::vec2 gridPos)
{
	if ((gridPos.x >= m_rows) || (gridPos.x < 0) || (gridPos.y >= m_cols) || (gridPos.y < 0))
	{
		//LOGERROR("Tried to access non-existent Tile with Grid Coords: %i, %i \n", gridPos.x, gridPos.y);
		return -1;
	}

	return EntityHandle(m_tileEntities->at(gridPos.y + gridPos.x * m_cols));
}

const TileComponent* TileMap::GetTileComponent(const EntityHandle e)
{
	return m_connector->GetEngine().GetWorld().get<TileComponent>(e);
}

const TOR::Transform* TileMap::GetTileTransformComponent(const EntityHandle e)
{
	return m_connector->GetEngine().GetWorld().get<TOR::Transform>(e);
}

const GridPositionComponent* TileMap::GetTileGridPosComponent(const EntityHandle e)
{
	return m_connector->GetEngine().GetWorld().get<GridPositionComponent>(e);
}

const BoxComponent* TileMap::GetTileBoxComponent(const EntityHandle e)
{
	return m_connector->GetEngine().GetWorld().get<BoxComponent>(e);
}
