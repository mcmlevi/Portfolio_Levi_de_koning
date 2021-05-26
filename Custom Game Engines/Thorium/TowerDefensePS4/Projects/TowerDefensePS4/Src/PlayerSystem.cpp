#include "pch.h"
#include "PlayerSystem.h"

#include "Core/MessageLogger.h"

PlayerSystem::PlayerSystem() : m_Gold(1), m_Lives(1), m_DoOnce(false)
{
	Register();
}

void PlayerSystem::OnUpdate(float dt)
{
	
	if (m_Lives <= 0 && m_DoOnce != true)
	{
		LOGDEBUG("Game Over");
		m_DoOnce = true;
	}
}

void PlayerSystem::AddGold(int value)
{
	m_Gold += value;
	printf("%i \n", m_Gold);
}

int PlayerSystem::GetGold()
{
	return m_Gold;
}

int PlayerSystem::GetLives()
{
	return m_Lives;
}

void PlayerSystem::AddLives(int value)
{
	m_Lives += value;
}
