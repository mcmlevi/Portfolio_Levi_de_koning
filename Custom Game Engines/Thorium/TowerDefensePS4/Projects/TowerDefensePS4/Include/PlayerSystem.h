#pragma once
#include "Core\Systems.h"
class PlayerSystem :
    public Th::Systems
{
public:
	PlayerSystem();
	~PlayerSystem() = default;
	void OnUpdate(float) override;
	void AddGold(int value);
	int GetGold();
	int GetLives();
	void AddLives(int value);
private:
	int m_Gold;
	int m_Lives;
	bool m_DoOnce;
};

