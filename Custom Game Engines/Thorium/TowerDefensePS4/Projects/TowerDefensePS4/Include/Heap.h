#pragma once
#include <vector>
#include "Tile.h"

class Heap {
public:
	Heap(int maxSize);
	~Heap();

	void Add(Tile* tile);
	void SortUp(Tile* tile);
	void sortDown(Tile* tile);

	void SwapItems(Tile* a, Tile* b);
	Tile* RemoveFirst();

	void UpdateItem(Tile* tile);
	bool Contains(Tile* tile);
	int count();

	Tile* getItem(int index);
private:
	std::vector<Tile*> items;
	int m_CurrentItemCount;
};