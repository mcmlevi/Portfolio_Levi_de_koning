#pragma once
#include <entt/single_include/entt/entt.hpp>
#include <glm/glm.hpp>

namespace Th {
	namespace Components {
		struct TransformComponent;
	}
}

enum class TargetMode
{
	First,
	Last,
	Strong,
	Close
};
struct TowerComponent
{
public:
	TowerComponent();
	~TowerComponent() = default;
	void Initialize(TargetMode targeting, int damage, float attackSpeed, float rotationSpeed, float range, std::vector<glm::vec3> spawnpoints);
	entt::entity m_TowerHead = entt::null;
	int GetDamage();
	float GetAttackSpeed();
	float GetRotationSpeed();
	float GetRange();

	bool AddEnemyToNearestList(entt::entity enemy);
	bool RemoveEnemyFromNearbyList(entt::entity enemy);
	TargetMode GetTargetMode() const;
	void SetTarget(entt::entity& enemy);
	entt::entity GetTarget() const;
	std::vector<entt::entity> GetEnemiesNearList() const;
	float GetTimer() const;
	float m_Timer;
	std::vector<glm::vec3> GetBulletSpawnPoints() const;
	int index = 0;
private:
	TargetMode m_TargetMode;

	int m_Damage;
	float m_AttackSpeed;
	float m_RotationSpeed;
	float m_Range;

	std::vector<entt::entity> m_EnemiesInRange;
	entt::entity m_Target;
	std::vector<glm::vec3> m_BulletSpawns;
	//float m_Timer;
};
