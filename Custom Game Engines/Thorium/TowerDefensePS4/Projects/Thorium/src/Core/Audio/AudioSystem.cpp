#include "Thpch.h"
#include <Core/Audio/AudioSystem.h>
#include <Core/Audio/AudioComponent.h>
#include <Components/TransformComponent.h>

#include "Core/Audio/AudioManager.h"

namespace Th
{
	AudioSystem::AudioSystem():Systems{}
	{
		Register();
	}

	AudioSystem::~AudioSystem()
	{
	}

	void AudioSystem::OnUpdate(float a_DeltaTime)
	{
		// update all audio components
		auto audioComponents = GetEntityRegister().view<Th::AudioComponent, Th::Components::TransformComponent>();
		for (auto entity : audioComponents)
		{
			Th::Components::TransformComponent& transform = audioComponents.get<Th::Components::TransformComponent>(entity);
			Th::AudioComponent& audioCmp = audioComponents.get<Th::AudioComponent>(entity);
			UpdateTransform(transform, audioCmp);
				
		}
		// update all listeners
		auto listeners = GetEntityRegister().view<Th::AudioListener, Th::Components::TransformComponent>();
		FMOD::Studio::System* studioSystem =  Th::AudioManager::GetInstance().GetStudioSystem();
		for (auto entity : listeners)
		{
			auto& listenerCmp = listeners.get<Th::AudioListener>(entity);
			Th::Components::TransformComponent& transform = listeners.get<Th::Components::TransformComponent>(entity);
			UpdateTransform(transform, *static_cast<AudioComponent*>(&listenerCmp));
			studioSystem->setListenerAttributes(0, &listenerCmp.m_attributes);
		}
		// update all sources
		auto sources = GetEntityRegister().view<Th::AudioSource, Th::Components::TransformComponent>();
		// Update sources
		for (auto entity : sources)
		{
			Th::Components::TransformComponent& transform = sources.get<Th::Components::TransformComponent>(entity);
			Th::AudioSource& audioCmp = sources.get<Th::AudioSource>(entity);
			UpdateTransform(transform, *static_cast<AudioComponent*>(&audioCmp));
		}
	}

	void AudioSystem::UpdateTransform(Th::Components::TransformComponent& transform, AudioComponent& audioCmp)
	{
		glm::vec3 prevPos(audioCmp.m_attributes.position.x, audioCmp.m_attributes.position.y, audioCmp.m_attributes.position.z);
		glm::mat4 world = transform.GetWorldMatrix();
		glm::vec3 position = transform.GetPosition();
		glm::vec3 forward = transform.GetForward();
		forward = glm::normalize(forward);
		glm::vec3 up = transform.GetUp();
		up = glm::normalize(up);

		audioCmp.m_attributes.position = VectorToFmod(position);
		audioCmp.m_attributes.forward = VectorToFmod(forward);
		audioCmp.m_attributes.velocity = { 0.0f, 0.0f, 0.0f };
		audioCmp.m_attributes.up = VectorToFmod(up);
	}
}
