#include "Thpch.h"
#include <Core/Audio/AudioComponent.h>
#include "Core/Audio/AudioManager.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/transform.hpp>
#include <Components/TransformComponent.h>

//void Th::AudioComponent::Update(float dt)
//{
// if (m_transform)
//	{
//		glm::vec3 prevPos(m_attributes.position.x, m_attributes.position.y, m_attributes.position.z);
//		glm::mat4 world = m_transform->GetWorldMatrix();
//		glm::vec3 position = world * glm::vec4();
//		glm::vec3 forward = m_transform->GetForward();
//		forward = glm::normalize(forward);
//		glm::vec3 up = m_transform->GetUp();
//		up = glm::normalize(up);
//
//		m_attributes.position = VectorToFmod(position);
//		m_attributes.forward = VectorToFmod(forward);
//		m_attributes.velocity = { 0.0f, 0.0f, 0.0f };
//		m_attributes.up = VectorToFmod(up);
//	}
//}


Th::AudioSource::AudioSource():AudioComponent{}
{
}

Th::AudioSource::~AudioSource()
{
}

bool Th::AudioSource::LoadEvent(const std::string& eventPath)
{
	FMOD::Studio::EventDescription* description = Th::AudioManager::GetInstance().LoadDescription(eventPath);
	if (description)
	{
		ERRCHECKAUDIO(description->createInstance(&m_event));
		if (m_event)
		{
			bool _is3D = false;
			ERRCHECKAUDIO(description->is3D(&_is3D));

			return true;
		}
	}

	return false;
}

void Th::AudioSource::Play() const
{
	ERRCHECKAUDIO(m_event->start());
}

void Th::AudioSource::Stop() const
{
	ERRCHECKAUDIO(m_event->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT));
}

bool Th::AudioSource::IsPlaying()
{
	ERRCHECKAUDIO(m_event->getPlaybackState(&m_state));
	return m_state == FMOD_STUDIO_PLAYBACK_PLAYING;
}

void Th::AudioSource::SetVolume(float volume) const
{
	m_event->setVolume(volume);
}

void Th::AudioSource::SetPitch(float pitch) const
{
	m_event->setPitch(pitch);
}

void Th::AudioSource::SetParameter(const std::string& name, float value) const
{
	ERRCHECKAUDIO(m_event->setParameterByName(name.c_str(), value));
}

void Th::AudioSource::UpdatePositionalData() const
{
	ERRCHECKAUDIO(m_event->set3DAttributes(&m_attributes));
}

Th::AudioListener::AudioListener() : AudioComponent{}
{
	Th::AudioManager::GetInstance().Set(this);
}

Th::AudioListener::~AudioListener()
{
	Th::AudioManager::GetInstance().Remove(this);
}
