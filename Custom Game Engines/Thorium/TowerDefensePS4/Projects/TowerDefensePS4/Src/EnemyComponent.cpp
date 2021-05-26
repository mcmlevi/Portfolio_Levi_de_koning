#include "pch.h"
#include "EnemyComponent.h"

#include <Core/MessageLogger.h>

#include "Components/TransformComponent.h"

EnemyComponent::EnemyComponent() : m_Speed(1), m_Health(1)
{
	
}

void EnemyComponent::Initialize(float speed, int health, int gold) {
	m_Speed = speed;
	m_Health = health;
	m_GoldWorth = gold;
	LOGDEBUG("Enemy created");
}

bool EnemyComponent::MoveTo(glm::vec3 pos, Th::Components::TransformComponent temp)
{
	if (glm::distance(temp.GetPosition(),moveto) < 0.05f)
	{
		glm::vec3 swap = moveto;
		moveto = next;
		next = swap;

		
		return true;
	}
	return false;
}

bool EnemyComponent::MoveToTarget(glm::vec3 target, Th::Components::TransformComponent* transform, float dt) {
	if (glm::distance(transform->GetPosition(), target) > 0.05f) {
		glm::vec3 newpos = transform->GetPosition() + glm::normalize(target - transform->GetPosition()) * dt * m_Speed;
		transform->SetPosition(newpos);
		return false;
	}
	else {
		return true;
	}
		
		//return false;
	
}


float EnemyComponent::GetSpeed()
{
	return m_Speed;
}

int EnemyComponent::GetHealth()
{
	return m_Health;
}

void EnemyComponent::ChangeHealth(int value)
{
	m_Health += value;
}

glm::vec3 EnemyComponent::GetMoveTo()
{
	return moveto;
}

int EnemyComponent::GetWorth() {
	return m_GoldWorth;
}
