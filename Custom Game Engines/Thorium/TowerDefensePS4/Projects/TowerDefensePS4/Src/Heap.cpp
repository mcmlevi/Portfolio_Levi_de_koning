#include "pch.h"
#include"Heap.h"

Heap::Heap(int maxSize) {
	items = std::vector<Tile*>(maxSize);
	m_CurrentItemCount = 0;
}

Heap::~Heap() {

}

void Heap::Add(Tile* tile) {
	tile->m_HeapIndex = m_CurrentItemCount;
	items[m_CurrentItemCount] = tile;
	SortUp(tile);
	m_CurrentItemCount++;
}

void Heap::SortUp(Tile* tile) {
	int parentIndex = (tile->m_HeapIndex - 1) / 2;
	while (true) {
		Tile* parentItem = items[parentIndex];
		if (tile->GetFCost() < parentItem->GetFCost()) {
			SwapItems(tile, parentItem);
		}
		else {
			break;
		}
		parentIndex = (tile->m_HeapIndex - 1) / 2;
	}
}

void Heap::sortDown(Tile* tile) {
	while (true) {
		int childIndexLeft = tile->m_HeapIndex * 2 + 1;
		int childIndexRight = tile->m_HeapIndex * 2 + 2;
		int swapIndex = 0;

		if (childIndexLeft < m_CurrentItemCount) {
			swapIndex = childIndexLeft;
			if (childIndexRight < m_CurrentItemCount) {
				if (items[childIndexLeft]->GetFCost() > items[childIndexRight]->GetFCost())
				{
					swapIndex = childIndexRight;
				}
			}
			if (tile->GetFCost() > items[swapIndex]->GetFCost())
			{
				SwapItems(tile, items[swapIndex]);
			}
			else {
				return;
			}
		}
		else {
			return;
		}
	}
}

void Heap::SwapItems(Tile* a, Tile* b) {
	items[a->m_HeapIndex] = b;
	items[b->m_HeapIndex] = a;
	int itemAheapIndex = a->m_HeapIndex;
	a->m_HeapIndex = b->m_HeapIndex;
	b->m_HeapIndex = itemAheapIndex;
}

Tile* Heap::RemoveFirst() {
	Tile* firstTile = items[0];
	m_CurrentItemCount--;
	items[0] = items[m_CurrentItemCount];
	items[0]->m_HeapIndex = 0;
	sortDown(items[0]);
	return firstTile;
}

void Heap::UpdateItem(Tile* tile) {
	SortUp(tile);
}

bool Heap::Contains(Tile* tile) {
	if (tile->m_HeapIndex <= items.size()) {
		return items[tile->m_HeapIndex] == tile ? true : false;
	}
	else return false;
		
}

int Heap::count() {
	return m_CurrentItemCount;
}

Tile* Heap::getItem(int index) {
	return items[index];
}
