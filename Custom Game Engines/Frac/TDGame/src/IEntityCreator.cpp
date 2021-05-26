#include "pch.h"
#include "IEntityCreator.h"

IEntityCreator::IEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager) {
	m_sceneManager = sceneManager;
	m_tileManager = tileManager;
}

IEntityCreator::~IEntityCreator() {

}

bool IEntityCreator::CheckAvailability(glm::vec2 position) {
	return m_tileManager->IsTileOccupied(position);
}
