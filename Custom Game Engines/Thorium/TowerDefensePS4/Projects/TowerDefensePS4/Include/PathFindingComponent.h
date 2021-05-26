#pragma once
#include <glm/glm.hpp>
#include "LevelManager.h"
#include <set>
#include <unordered_set>


struct PathFindingComponent {
public:
	PathFindingComponent(LevelManager* levelManager);
	~PathFindingComponent();
	void FindPath(glm::vec2 startPos, glm::vec2 targetPos);

	std::vector<Tile*> m_LastRetracedPath;
private:
	int GetDistance(Tile* tileA, Tile* tileB);
	void RetracePath(Tile* startNode, Tile* endNode);
	LevelManager* m_LevelManager;
	bool pathSucces = false;
	
};