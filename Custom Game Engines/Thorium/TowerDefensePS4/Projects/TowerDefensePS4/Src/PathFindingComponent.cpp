#include "pch.h"
#include "PathFindingComponent.h"
#include <Core/WorldManager.h>
#include <Components/TransformComponent.h>
#include <chrono>
#include "Heap.h"

PathFindingComponent::PathFindingComponent(LevelManager* levelManager) {
	m_LevelManager = levelManager;

}

PathFindingComponent::~PathFindingComponent() {

}

void PathFindingComponent::FindPath(glm::vec2 startPos, glm::vec2 targetPos) {
	//Time checking
	auto t1 = std::chrono::high_resolution_clock::now();

	Tile* startTile = &m_LevelManager->m_AllTiles2D(startPos.x, startPos.y);
	Tile* TargetTile = &m_LevelManager->m_AllTiles2D(targetPos.x, targetPos.y);
	
	Heap openSet = Heap(59 * 59);
	std::unordered_set<Tile*> closedSet; //TODO research unordered set
	openSet.Add(startTile);

	while (openSet.count() > 0) {
		Tile* currentTile = openSet.RemoveFirst();
		closedSet.insert(currentTile);

		if (currentTile == TargetTile) {
			RetracePath(startTile, TargetTile);
			auto t2 = std::chrono::high_resolution_clock::now();
			float duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			std::cout << "found path, took: " << duration / 1000 << " milliseconds." << std::endl;
			pathSucces = true;
			//std::cout << pathSucces << std::endl;
			return;
		}

		for (unsigned int i = 0; i < 8; i++) {
			if (currentTile->m_SideTiles[i] != nullptr) {
				if (currentTile->m_SideTiles[i]->m_Initialized) {
					Tile* neighbor = currentTile->m_SideTiles[i];
					if (!neighbor->m_Passable || closedSet.find(neighbor) != closedSet.end()) {
						continue;
					}

					int newMovementCostToNeighbor = currentTile->gCost + GetDistance(currentTile, neighbor) + neighbor->movementPenalty;
					if (newMovementCostToNeighbor < neighbor->gCost || !openSet.Contains(neighbor)) {
						neighbor->gCost = newMovementCostToNeighbor;
						neighbor->hCost = GetDistance(neighbor, TargetTile);
						neighbor->parrent = currentTile;
						if (!openSet.Contains(neighbor)) {
							openSet.Add(neighbor);
						}
						else {
							openSet.UpdateItem(neighbor);
						}
					}
				}
			}
		}
	}
	pathSucces = false;
	std::cout << pathSucces << std::endl;
}

int PathFindingComponent::GetDistance(Tile* tileA, Tile* tileB) {
	int dstX = glm::abs(tileA->m_TilePosition.x - tileB->m_TilePosition.x);
	int dstY = glm::abs(tileA->m_TilePosition.z - tileB->m_TilePosition.z);
	if (dstX > dstY) {
		return 14 * dstY + 10 * (dstX - dstY);
	}
	else {
		return 14 * dstX + 10 * (dstY - dstX);
	}
}

void PathFindingComponent::RetracePath(Tile* startNode, Tile* endNode) {
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();
	m_LastRetracedPath.clear();
	Tile* currentNode = endNode;

	while (currentNode != startNode) {
		m_LastRetracedPath.push_back(currentNode);
		currentNode = currentNode->parrent;
	}
	std::reverse(m_LastRetracedPath.begin(), m_LastRetracedPath.end());
}
