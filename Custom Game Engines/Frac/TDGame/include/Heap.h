#pragma once
#include <vector>
#include "Components.h"
#include "Core/EngineCore.h"

class Heap {
public:
	Heap(int maxSize, Frac::EngineConnector* connector);
	~Heap();

	void Add(EntityHandle* entity);
	void SortUp(EntityHandle* entity);
	void sortDown(EntityHandle* entity);

	void SwapItems(EntityHandle* a, EntityHandle* b);
	EntityHandle* RemoveFirst();

	void UpdateItem(EntityHandle* entity);
	bool Contains(EntityHandle* entity);
	int count();

	EntityHandle* getItem(int index);
private:
	std::vector<EntityHandle*> items;
	int m_CurrentItemCount;

	Frac::EngineConnector* m_connector;
};