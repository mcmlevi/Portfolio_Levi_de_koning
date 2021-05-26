#include <Thpch.h>
#include <Core/Audio/AudioManager.h>
#include <Core/MessageLogger.h>
#include <Core/Audio/AudioComponent.h>

#include <fmod_errors.h>
#include <fmod_studio.hpp>

#if PLATFORM_PS4

#endif

void ERRCHECK_fnAudio(FMOD_RESULT result, const char* file, int line)
{
	if (result != FMOD_OK)
	{
		Th::MessageLogger::GetInstance().LogError("%s(%d): FMOD error %d - %s", file, line, result, FMOD_ErrorString(result));
	}
}

FMOD_VECTOR VectorToFmod(const glm::vec3& vector)
{
	return FMOD_VECTOR{ vector.x , vector.y, vector.z };
}

namespace Th
{
	Th::AudioManager& Th::AudioManager::GetInstance()
	{
		static AudioManager manager{};
		return manager;
	}

	void Th::AudioManager::Initialize()
	{
#if PLATFORM_PS4
		
#if _DEBUG
		
#else
		
#endif
		//TODO: Fix correct devkit libraries.
		/*SceKernelModule module_id = sceKernelLoadStartModule(fmodModulePath, 0, 0, 0, NULL, NULL);
		if (module_id < SCE_OK)
		{
			LOGERROR("Failed to boot the fmod module!");
			return;
		}
		module_id = sceKernelLoadStartModule(fmodStudioModulePath, 0, 0, 0, NULL, NULL);
		if (module_id < SCE_OK)
		{
			LOGERROR("Failed to boot the fmod studio module!");
			return;
		}*/
#endif

		ERRCHECKAUDIO(FMOD::Studio::System::create(&m_studioSystem));
		ERRCHECKAUDIO(m_studioSystem->getCoreSystem(&m_system));
		ERRCHECKAUDIO(m_system->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));
#if defined(_DEBUG) && defined(PLATFORM_WIN64)
		ERRCHECKAUDIO(m_studioSystem->initialize(128, FMOD_STUDIO_INIT_LIVEUPDATE, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr));
#else
		ERRCHECKAUDIO(m_studioSystem->initialize(128, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, nullptr));
#endif

		Th::MessageLogger::GetInstance().LogDebug("Fmod Initialization Complete");
	}

	void Th::AudioManager::ShutDown()
	{
		ERRCHECKAUDIO(m_studioSystem->unloadAll());
		ERRCHECKAUDIO(m_studioSystem->flushCommands());
		ERRCHECKAUDIO(m_studioSystem->release());
	}

	void Th::AudioManager::Update(float dt)
	{
		// Update one-off sounds
		for (auto itr = m_events.begin(); itr != m_events.end(); )
		{
			FMOD::Studio::EventInstance* instance = *itr;
			FMOD_STUDIO_PLAYBACK_STATE state;
			ERRCHECKAUDIO(instance->getPlaybackState(&state));
			if (state == FMOD_STUDIO_PLAYBACK_STOPPED)
			{
				instance->release();
				itr = m_events.erase(itr);
			}
			else
			{
				++itr;
			}
		}

		for (auto& itr : m_descriptions)
		{
			itr.second.Cooldown -= dt;
		}

		ERRCHECKAUDIO(m_studioSystem->update());
	}

	void Th::AudioManager::LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags)
	{
		auto foundIt = m_banks.find(bankName);
		if (foundIt != m_banks.end())
			return;


		FMOD::Studio::Bank* bank;
		ERRCHECKAUDIO(m_studioSystem->loadBankFile(bankName.c_str(), flags, &bank));
		if (bank)
			m_banks[bankName] = bank;
	}

	void AudioManager::PlayEvent(const std::string& eventName, const glm::vec3& position, float volume)
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
			FMOD::Studio::EventInstance* instance = nullptr;
			ERRCHECKAUDIO(description->createInstance(&instance));
			if (instance)
			{
				m_events.push_back(instance);
				ERRCHECKAUDIO(instance->start());
				ERRCHECKAUDIO(instance->setVolume(volume));
				m_descriptions[eventName].Cooldown = 0.066f;
				bool is3D = false;
				ERRCHECKAUDIO(description->is3D(&is3D));
				if (is3D)
				{
					FMOD_3D_ATTRIBUTES attributes;
					attributes.position = VectorToFmod(position);
					attributes.forward = { 0.0f, 0.0f, 1.0f };
					attributes.up = { 0.0f, 1.0f, 0.0f };
					attributes.velocity = { 0.0f, 0.0f, 0.0f };
					instance->set3DAttributes(&attributes);
				}
			}
			else
			{
				LOGERROR("Unable to create istance of sound event: %s", eventName.c_str());
			}
		}
		else
		{
			LOGERROR("Unable to locate sound event description: %s", eventName.c_str());
		}
	}

	void Th::AudioManager::StopEvent(const std::string& eventName)
	{
		FMOD::Studio::EventDescription* des;
		char path[256];
		int retrieved;
		for (auto ev : m_events)
		{
			ERRCHECKAUDIO(ev->getDescription(&des));
			ERRCHECKAUDIO(des->getPath(path, 256, &retrieved));
			if (strcmp(path, eventName.c_str()) == 0)
			{
				ev->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
			}
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
	void AudioManager::Set(AudioListener* listener)
	{
		m_listener = listener;
	}

	void AudioManager::Remove(AudioListener* listener)
	{
		m_listener = nullptr;
	}

	void AudioManager::SetVCAVolume(const std::string& vcaName, float volume)
	{
		auto it = m_vcas.find(vcaName);
		if (it != m_vcas.end())
		{
			it->second->setVolume(volume);
		}
		else
		{
			FMOD::Studio::VCA* vca;
			ERRCHECKAUDIO(m_studioSystem->getVCA(vcaName.c_str(), &vca));
			if (vca)
			{
				m_vcas[vcaName] = vca;
				m_vcas[vcaName]->setVolume(volume);
			}
		}
	}
}


