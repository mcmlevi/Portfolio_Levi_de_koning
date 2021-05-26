#include "pch.h"
#include "TileMap.h"


TileMap::TileMap(unsigned int width, unsigned int height) : m_Width(width), m_Height(height) {
	m_Tiles.resize(width * height);
}

TileMap::~TileMap() {
	
}

Tile& TileMap::operator()(unsigned int x, unsigned int y) {
	return m_Tiles[x + y * m_Width];
}

Tile& TileMap::operator()(glm::vec2 position) {
	return m_Tiles[int(position.x + position.y * m_Width)];
}

const Tile& TileMap::operator()(unsigned int x, unsigned int y) const {
	return m_Tiles[x + y * m_Width];
}
