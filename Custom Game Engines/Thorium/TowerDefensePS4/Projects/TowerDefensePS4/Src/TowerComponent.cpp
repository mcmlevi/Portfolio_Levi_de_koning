#include "pch.h"
#include "TowerComponent.h"
#include <Core/MessageLogger.h>
#include "Components/TransformComponent.h"
TowerComponent::TowerComponent() :m_TargetMode(TargetMode::First), m_Damage(1), m_AttackSpeed(1.0f), m_RotationSpeed(1.0f), m_Timer(0)
{

}

void TowerComponent::Initialize(TargetMode targetmode, int damage, float attackSpeed, float rotationSpeed, float range, std::vector<glm::vec3> spawnpoints)
{
	m_Damage = damage;
	m_AttackSpeed = attackSpeed;
	m_RotationSpeed = rotationSpeed;
	m_TargetMode = targetmode;
	m_Range = range;
	m_BulletSpawns = spawnpoints;
}

int TowerComponent::GetDamage()
{
	return m_Damage;
}

float TowerComponent::GetAttackSpeed()
{
	return m_AttackSpeed;
}

float TowerComponent::GetRotationSpeed()
{
	return m_RotationSpeed;
}

float TowerComponent::GetRange()
{
	return m_Range;
}

bool TowerComponent::AddEnemyToNearestList(entt::entity enemy)
{

	auto it = m_EnemiesInRange.begin();

	it = std::find(m_EnemiesInRange.begin(), m_EnemiesInRange.end(), enemy);
	if (it != m_EnemiesInRange.end())
	{

		return false;
	}
	m_EnemiesInRange.push_back(enemy);
	return true;
}

bool TowerComponent::RemoveEnemyFromNearbyList(entt::entity enemy)
{

	auto it = m_EnemiesInRange.begin();

	it = std::find(m_EnemiesInRange.begin(), m_EnemiesInRange.end(), enemy);
	if (it != m_EnemiesInRange.end())
	{
		m_EnemiesInRange.erase(it);

		//LOGWARNING("Enemy Removed from List");
		return true;
	}


	//change to index
	//m_EnemiesInRange.erase(m_EnemiesInRange.begin() + 1);
	return false;
}

TargetMode TowerComponent::GetTargetMode() const
{
	return m_TargetMode;
}

void TowerComponent::SetTarget(entt::entity& enemy)
{
	m_Target = enemy;
}

entt::entity TowerComponent::GetTarget() const
{
	return m_Target;
}

std::vector<entt::entity> TowerComponent::GetEnemiesNearList() const
{
	return m_EnemiesInRange;
}

float TowerComponent::GetTimer() const
{
	return m_Timer;
}

std::vector<glm::vec3> TowerComponent::GetBulletSpawnPoints() const
{
	return m_BulletSpawns;
}

