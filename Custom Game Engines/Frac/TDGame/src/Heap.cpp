#include "pch.h"
#include"Heap.h"

Heap::Heap(int maxSize, Frac::EngineConnector* connector) {
	items = std::vector<EntityHandle*>(maxSize);
	m_CurrentItemCount = 0;
	m_connector = connector;
}

Heap::~Heap() {

}

void Heap::Add(EntityHandle* entity) {
	TileComponent* entityTileC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*entity);
	entityTileC->m_aStar.m_heapIndex = m_CurrentItemCount;
	items[m_CurrentItemCount] = entity;
	SortUp(entity);
	m_CurrentItemCount++;
}

void Heap::SortUp(EntityHandle* entity) {
	TileComponent* entityTileC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*entity);
	int parentIndex = (entityTileC->m_aStar.m_heapIndex - 1) / 2;
	while (true) {
		std::cout << "nieuwe kanker" << std::endl;
		EntityHandle* parentItem = items[parentIndex];
		TileComponent* parentItemTC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*items[parentIndex]);
		if (entityTileC->m_aStar.GetFCost() < parentItemTC->m_aStar.GetFCost()) {
			SwapItems(entity, parentItem);
		}
		else {
			break;
		}
		parentIndex = (entityTileC->m_aStar.m_heapIndex - 1) / 2;
	}
}

void Heap::sortDown(EntityHandle* entity) {
	TileComponent* tile = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*entity);

	while (true) {
		int childIndexLeft = tile->m_aStar.m_heapIndex * 2 + 1;
		int childIndexRight = tile->m_aStar.m_heapIndex * 2 + 2;
		int swapIndex = 0;

		TileComponent* childIndexLefttile = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*items[childIndexLeft]);
		TileComponent* childIndexRighttile = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*items[childIndexRight]);

		if (childIndexLeft < m_CurrentItemCount) {
			swapIndex = childIndexLeft;
			if (childIndexRight < m_CurrentItemCount) {
				
				if (childIndexLefttile->m_aStar.GetFCost() > childIndexRighttile->m_aStar.GetFCost())
				{
					swapIndex = childIndexRight;
				}
			}
			TileComponent* swapIndexTile = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*items[swapIndex]);
			if (tile->m_aStar.GetFCost() > swapIndexTile->m_aStar.GetFCost())
			{
				SwapItems(entity, items[swapIndex]);
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

void Heap::SwapItems(EntityHandle* a, EntityHandle* b) {
	TileComponent* aTC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*a);
	TileComponent* bTC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*b);
	items[aTC->m_aStar.m_heapIndex] = b;
	items[bTC->m_aStar.m_heapIndex] = a;
	int itemAheapIndex = aTC->m_aStar.m_heapIndex;
	aTC->m_aStar.m_heapIndex = bTC->m_aStar.m_heapIndex;
	bTC->m_aStar.m_heapIndex = itemAheapIndex;
}

EntityHandle* Heap::RemoveFirst() {
	std::cout << m_CurrentItemCount << std::endl;
	EntityHandle* firstTileEH = items[0];
	m_CurrentItemCount--;
	items[0] = items[m_CurrentItemCount];
	std::cout << m_CurrentItemCount << std::endl;
	EntityHandle* newFirstTileEH = items[0];
	TileComponent* entityTileC = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*newFirstTileEH);
	entityTileC->m_aStar.m_heapIndex = 0;
	sortDown(items[0]);
	return firstTileEH;
}

void Heap::UpdateItem(EntityHandle* entity) {
	SortUp(entity);
}

bool Heap::Contains(EntityHandle* entity) {
	TileComponent* tc = m_connector->GetEngine().GetWorld().get_mutable<TileComponent>(*entity);

	if (tc->m_aStar.m_heapIndex <= items.size()) {
		return items[tc->m_aStar.m_heapIndex] == entity ? true : false;
	}
	else return false;

}

int Heap::count() {
	return m_CurrentItemCount;
}

EntityHandle* Heap::getItem(int index) {
	return items[index];
}
