#pragma once
#include <fmod_common.h>
#include <fmod_studio.hpp>


namespace Th
{
	class AudioComponent
	{
	public:
		AudioComponent() = default;
		virtual ~AudioComponent() = default;
		FMOD_3D_ATTRIBUTES	m_attributes;
	};

	class AudioSource : public AudioComponent
	{
	public:
	
		AudioSource();

		virtual ~AudioSource() override;

		bool LoadEvent(const std::string& eventPath);

		void Play() const;

		void Stop() const;

		bool IsPlaying();

		void SetVolume(float volume) const;

		void SetPitch(float pitch) const;

		void SetParameter(const std::string& name, float value) const;

		void UpdatePositionalData() const;
	protected:
		FMOD::Studio::EventInstance* m_event = nullptr;
		FMOD_STUDIO_PLAYBACK_STATE		m_state;
	};

	class AudioListener : public AudioComponent
	{
	public:
		AudioListener();
		virtual ~AudioListener();
	};
}
