#include <pch.h>
#include <Audio/AudioManager.h>
#include <Audio/AudioComponent.h>
#include "CoreRenderAPI/Components/Transform.h"

#include <FMOD/core/fmod_errors.h>
#include <FMOD/studio/fmod_studio.hpp>

#include "Core/FileIO.h"

void ERRCHECK_fnAudio(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		LOGINFO("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

namespace Frac
{
	AudioManager::AudioManager(ECSWrapper& a_flecsWorld) : 
		m_flecsWorld(a_flecsWorld)
	{
		ERRCHECKAUDIO(FMOD::Studio::System::create(&m_studioSystem));
		ERRCHECKAUDIO(m_studioSystem->getCoreSystem(&m_system));
		ERRCHECKAUDIO(m_system->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));
		//ERRCHECKAUDIO(m_studioSystem->initialize(128, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr));
		ERRCHECKAUDIO(m_studioSystem->initialize(128, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr));

		std::function<void(EntityHandle, TOR::Transform&, Frac::AudioSourceComponent&)> updateAudio = [this](EntityHandle entity, TOR::Transform& transform, Frac::AudioSourceComponent& audioSourceComponent)
		{
			// TODO release sound
			/*if (audioSourceComponent.event) {
				FMOD_STUDIO_PLAYBACK_STATE state;
				ERRCHECKAUDIO(audioSourceComponent.event->getPlaybackState(&state));
				if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
				{
					audioSourceComponent.event->release();
				}
			}*/
		};

		m_flecsWorld.makeSystem<TOR::Transform, Frac::AudioSourceComponent>(updateAudio, ePipelineStage::postUpdate, "Update Audio System");

		LOGINFO("Fmod Initialization Complete");
	}

	void AudioManager::ShutDown()
	{
		ERRCHECKAUDIO(m_studioSystem->unloadAll());
		ERRCHECKAUDIO(m_studioSystem->flushCommands());
		ERRCHECKAUDIO(m_studioSystem->release());
	}

	void AudioManager::Update(float dt)
	{
		for (auto& itr : m_descriptions)
		{
			itr.second.Cooldown -= dt;
		}

		ERRCHECKAUDIO(m_studioSystem->update());
	}

	void AudioManager::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto foundIt = m_banks.find(FileIO::GetPathFromWildcard(bankName));
		if (foundIt != m_banks.end())
			return;

		FMOD::Studio::Bank* bank;
		ERRCHECKAUDIO(m_studioSystem->loadBankFile(FileIO::GetPathFromWildcard(bankName).c_str(), flags, &bank));
		if (bank)
			m_banks[bankName] = bank;
	}

	void AudioManager::PlayEvent(AudioSourceComponent& audioSourceComponent, const std::string& eventName, float volume)
	{
		FMOD::Studio::EventDescription* description = nullptr;
		auto found = m_descriptions.find(eventName);
		if (found != m_descriptions.end())
		{
			description = found->second.Description;
			if (found->second.Cooldown > 0.0f)
				return;
		}
		else
		{
			ERRCHECKAUDIO(m_studioSystem->getEvent(eventName.c_str(), &description));
			m_descriptions[eventName].Description = description;
		}

		if (description)
		{
			if (!audioSourceComponent.event) {
				ERRCHECKAUDIO(description->createInstance(&audioSourceComponent.event));
				if (audioSourceComponent.event)
				{
					ERRCHECKAUDIO(audioSourceComponent.event->start());
					ERRCHECKAUDIO(audioSourceComponent.event->setVolume(volume));
					m_descriptions[eventName].Cooldown = 0.066f;
					bool is3D = false;
					ERRCHECKAUDIO(description->is3D(&is3D));
					if (is3D)
					{
						FMOD_3D_ATTRIBUTES attributes;
						//attributes.position = VectorToFmod(position);
						attributes.forward = { 0.0f, 0.0f, 1.0f };
						attributes.up = { 0.0f, 1.0f, 0.0f };
						attributes.velocity = { 0.0f, 0.0f, 0.0f };
						audioSourceComponent.event->set3DAttributes(&attributes);
					}
				}
				else
				{
					LOGERROR("Unable to create istance of sound event: %s", eventName.c_str());
				}
			}
		}
		else
		{
			LOGERROR("Unable to locate sound event description: %s", eventName.c_str());
		}
	}

	FMOD::Studio::EventDescription* AudioManager::LoadDescription(const std::string& eventName)
	{
		auto found = m_descriptions.find(eventName);
		if (found != m_descriptions.end())
			return found->second.Description;

		FMOD::Studio::EventDescription* description = nullptr;
		ERRCHECKAUDIO(m_studioSystem->getEvent(eventName.c_str(), &description));
		m_descriptions[eventName].Description = description;
		return description;
	}

	FMOD::Studio::System* AudioManager::GetStudioSystem()
	{
		return m_studioSystem;
	}
	//void AudioManager::Set(AudioListener* listener)
	//{
	//	m_listener = listener;
	//}

	//void AudioManager::Remove(AudioListener* listener)
	//{
	//	m_listener = nullptr;
	//}
} // namespace Frac


