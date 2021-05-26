#include "pch.h"
#include "ProjectileSystem.h"

#include "ProjectileComponent.h"
#include "EnemyComponent.h"
#include "Core/MessageLogger.h"
#include "Core/Audio/AudioManager.h"

ProjectileSystem::ProjectileSystem()
{
	Register();
}

void ProjectileSystem::OnUpdate(float dt)
{
	for (auto entity : GetEntityRegister().view<ProjectileComponent>())
	{
		ProjectileComponent& bullet = GetEntityRegister().get<ProjectileComponent>(entity);
		Th::TransformComponent& transform = GetEntityRegister().get<Th::TransformComponent>(entity);

		glm::vec3 pos = transform.GetPosition();
		
		entt::entity target = bullet.GetTarget();
		if (GetEntityRegister().valid(target))
		{
			EnemyComponent& targetcomp = GetEntityRegister().get<EnemyComponent>(target);
			Th::TransformComponent& transformcomp = GetEntityRegister().get<Th::TransformComponent>(target);

			glm::vec3 targetpos = transformcomp.GetPosition();
			targetpos += glm::vec3(0, 0.2f, 0);
			glm::vec3 direction = glm::normalize(targetpos - pos);

			transform.SetPosition(pos + direction * bullet.GetSpeed() * dt);
			if (glm::distance(pos, targetpos) < 1.0f)
			{
				targetcomp.ChangeHealth(-bullet.GetDamage());
				Th::AudioManager& audioManager = Th::AudioManager::GetInstance();
				audioManager.PlayEvent("event:/FX/hit",transformcomp.GetPosition(), 0.5f);

				DeleteEntity(entity);				
			}
		}else
		{
			DeleteEntity(entity);
		}
	
	}	
}
