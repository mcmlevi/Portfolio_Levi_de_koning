#pragma once
#include <Core/Systems.h>

namespace Th
{
	class AudioComponent;
	class AudioSystem : public Systems
	{
	public:
		/**
		 * \brief Setup the audio system. automatically subscribes itself.
		 */
		AudioSystem();
		~AudioSystem();

	public:
		/**
		 * \brief Update the data in the audio component attributes and their data.
		 */
		void OnUpdate(float) override;
	private:
		void UpdateTransform(Th::Components::TransformComponent& transform, AudioComponent& audioCmp);
	};

}

