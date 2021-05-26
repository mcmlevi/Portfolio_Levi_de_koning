#include "pch.h"
#include"Tile.h"
#include <Core/WorldManager.h>
#include "InteractAbleComponent.h"

Tile::Tile() : m_TilePosition(glm::vec3(0, 0, 0)), m_IsMainTile(false), m_Empty(true), m_Initialized(false) {
	//Creating the tile without position setting and model setting, not initialized but created
}

Tile::Tile(glm::vec3 position, entt::entity parent) : m_TilePosition(position), m_Initialized(true) {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	TransformComponent transform(m_TilePosition, parent);
	m_TileInteractableEntity = entt::entity(worldManager.CreateEntity("TileInteractable", transform));
	InteractAbleComponent interactAbleComponent;
	worldManager.GetEntityRegister().emplace<InteractAbleComponent>(m_TileInteractableEntity, interactAbleComponent);
	movementPenalty = 0;
	m_PreferredTile = false;
}

Tile::Tile(glm::vec3 position, ModelComponent* modelComponent, entt::entity parent) : m_TilePosition(position), m_Initialized(true) {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	TransformComponent transform(m_TilePosition, parent);
	m_TileInteractableEntity = entt::entity(worldManager.CreateEntity("TileInteractable", transform));
	InteractAbleComponent interactAbleComponent;
	worldManager.GetEntityRegister().emplace<InteractAbleComponent>(m_TileInteractableEntity, interactAbleComponent);
	SetTileEntity(modelComponent, parent);
	movementPenalty = 0;
	m_PreferredTile = false;
}

Tile::~Tile() {

}

bool Tile::IsEmpty() {
	return m_Empty;
}

void Tile::SetSideTile(Tile* tile, int position) {
	if (position < 8) {
		m_SideTiles[position] = tile;
	}
	else {
		std::cout << "impossible index detected while adding tile, should be under 8";
	}
}

void Tile::SetTileEntity(ModelComponent* modelComponent, entt::entity parent) {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	TransformComponent transform(m_TilePosition, parent);
	m_TileEntity = entt::entity(worldManager.CreateEntity("Tile", transform));
	worldManager.GetEntityRegister().emplace<ModelComponent>(m_TileEntity, *modelComponent);
}

void Tile::SetInteractableEntity(ModelComponent* modelComponent) {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	if (m_HasInteractablePlaced == false) {
		worldManager.GetEntityRegister().emplace<ModelComponent>(m_TileInteractableEntity, *modelComponent);
		m_HasInteractablePlaced = true;
	}

	if (m_HasInteractablePlaced == true) {
		// create new model component

		//set interactable model component to new component.
	}
}

void Tile::SetTileType(int type) {
	currentTileType = TileType(type);
}
