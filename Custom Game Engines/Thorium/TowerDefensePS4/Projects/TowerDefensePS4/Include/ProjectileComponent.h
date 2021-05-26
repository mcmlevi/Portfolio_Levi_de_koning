#pragma once
#include <entt/single_include/entt/entt.hpp>
#include <glm/glm.hpp>
struct ProjectileComponent
{
public:
	ProjectileComponent();
	
	~ProjectileComponent() = default;
	void Initialize(float speed, float rotationSpeed, int damage, entt::entity target);
	entt::entity GetTarget() const;
	float GetSpeed() const;
	int GetDamage() const;
	
private:
	float m_Speed;
	float m_RotationSpeed;
	int m_Damage;
	entt::entity m_Target;
};

