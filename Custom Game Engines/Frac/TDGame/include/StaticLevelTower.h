#pragma once
#include "StaticLevelInteractable.h"
struct StaticLevelTower : public StaticLevelInteractable {
public:
	int m_bulletType;
	float m_fireRate;
	int m_towerHealth;
private:
};