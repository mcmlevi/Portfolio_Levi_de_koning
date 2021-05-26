#pragma once
#include <glm/glm.hpp>
#include "Tile.h"
class TileMap {
public:

	TileMap(unsigned int width = 0, unsigned int height = 0);
	~TileMap();
	Tile& operator()(unsigned int x, unsigned int y);
	Tile& operator()(glm::vec2 position);
	const Tile& operator()(unsigned int x, unsigned int y)const;
private:
	unsigned int m_Width;
	unsigned int m_Height;

	std::vector<Tile> m_Tiles;
	//Tile* m_Tiles;
};
