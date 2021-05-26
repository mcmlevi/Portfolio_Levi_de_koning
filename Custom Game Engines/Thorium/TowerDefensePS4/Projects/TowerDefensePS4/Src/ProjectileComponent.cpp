#include "pch.h"
#include "ProjectileComponent.h"

ProjectileComponent::ProjectileComponent(): m_Speed(1), m_RotationSpeed(1), m_Damage(1)
{
	
}

void ProjectileComponent::Initialize(float speed, float rotationSpeed, int damage, entt::entity target)
{
	
	m_Speed = speed;
	m_RotationSpeed = rotationSpeed;
	m_Damage = damage;
	m_Target = target;
}

entt::entity ProjectileComponent::GetTarget() const
{
	return m_Target;
}

float ProjectileComponent::GetSpeed() const
{
	return m_Speed;
}

int ProjectileComponent::GetDamage() const
{
	return m_Damage;
}
