#pragma once
#include <string>
#include <unordered_map>

#include <FMOD/studio/fmod_studio.hpp>

#include <glm/vec3.hpp>

void ERRCHECK_fnAudio(FMOD_RESULT result, const char* file, int line);
#define ERRCHECKAUDIO(_result) ERRCHECK_fnAudio(_result, __FILE__, __LINE__)

namespace Frac
{
	class AudioSource;
	class AudioListener;
	class ECSWrapper;
	struct AudioSourceComponent;
	
	class AudioManager
	{
	public:
		AudioManager(ECSWrapper& a_flecsWorld);
		~AudioManager() = default;
		
		/**
		 * \brief Should be called when closing the engine to make sure everything gets properly deleted.
		 */
		void ShutDown();
		/**
		 * \brief Update function for the audio manager should be called every frame to update the playback state of the audio
		 * \param deltaTime the time since last frame
		 */
		void Update(float deltaTime);
		/**
		 * \brief Load an audio bank from file.
		 * \param bankName the file location of the .bank file.
		 * \param flags how to load the bank into FMOD default should be fine for most use cases
		 */
		void LoadBank(const std::string& bankName, FMOD_STUDIO_LOAD_BANK_FLAGS flags = FMOD_STUDIO_LOAD_BANK_NORMAL);
		
		/**
		 * \brief Play an audio event from a component
		 * \param audioSourceComponent The component you want the audio to be played from
		 * \param eventName The name of the event to play
		 * \param volume the volume of the sound 
		 */
		void PlayEvent(AudioSourceComponent& audioSourceComponent, const std::string& eventName, float volume = 1.0f);
		/**
		* 
		 * \brief Stop playing a certain event
		 * \param eventName the event to stop playing
		 */
		//void StopEvent(const std::string& eventName); //TODO

		/**
		 * \brief Get the description of a certain event.
		 * \param eventName The name of the event
		 * \return The description of a certain event with all information about that event.
		 */
		FMOD::Studio::EventDescription* LoadDescription(const std::string& eventName);
		/**
		 * \brief Get a pointer to the studio system
		 * \return The studio system pointer
		 */
		FMOD::Studio::System* GetStudioSystem();

		/**
		 * \brief Set a listener to the manager. There can only be 1 listener
		 * \param listener the listener to add
		 */
		//void Set(AudioListener* listener);

		/**
		 * \brief Remove the listener from the manager.
		 * \param listener The listener to remove
		 */
		//void Remove(AudioListener* listener);
		
	private:
		struct DescriptionEntry
		{
			FMOD::Studio::EventDescription* Description;
			float Cooldown;
		};

		FMOD::System* m_system = nullptr;
		FMOD::Studio::System* m_studioSystem = nullptr;

		std::unordered_map<std::string, DescriptionEntry> m_descriptions;
		std::unordered_map<std::string, FMOD::Studio::Bank*> m_banks;
		//AudioListener* m_listener = nullptr;

		ECSWrapper& m_flecsWorld;
		
	};
} // namespace Frac
