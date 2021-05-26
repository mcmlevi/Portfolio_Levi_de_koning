#pragma once
#include <glm/glm.hpp>
#include <Components/ModelComponent.h>
#include "Core\Systems.h"
using namespace Th::Components;
class Tile {
public:
	Tile();
	Tile(glm::vec3 position, entt::entity parent);
	Tile(glm::vec3 position, ModelComponent* modelComponent, entt::entity parent);
	~Tile();

	bool IsEmpty();
	bool m_Initialized;
	bool m_IsMainTile;
	int m_MainTileNeighbors = 0;
	int m_ConnectiveTileNeighbors = 0;

	entt::entity m_TileEntity{};
	glm::vec3 m_TilePosition; // used before model position can be used
	entt::entity m_TileInteractableEntity{};
	entt::entity m_TowerBase{};
	entt::entity m_TowerHead{};
	entt::entity m_TileTowerConnection{};

	void SetSideTile(Tile* tile, int position);
	void SetTileEntity(ModelComponent* modelComponent, entt::entity parent);
	void SetInteractableEntity(ModelComponent* modelComponent);
	enum TileType {
		FlatTile,
		DiagonalTile,
		CornerTileDownWards,
		CornerTileUpwards,
	};
	TileType currentTileType;

	void SetTileType(int type);

	//Pathfinding data
	Tile* parrent = nullptr;

	bool m_Passable = true;
	bool m_HasInteractablePlaced = false;
	int gCost;
	int hCost;
	int movementPenalty = 0;
	bool m_PreferredTile;
	int GetFCost() {
		return gCost + hCost;
	}

	Tile* m_SideTiles[8] = {
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		nullptr
	};

	int m_HeapIndex = -1;

private:
	bool m_Empty; //bool for containing a interactable
	//float m_Height;


};
