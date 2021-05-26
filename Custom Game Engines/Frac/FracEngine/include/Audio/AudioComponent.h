#pragma once
#include <FMOD/core/fmod_common.h>
#include <FMOD/studio/fmod_studio.hpp>

namespace Frac
{
	struct AudioSourceComponent
	{
		FMOD::Studio::EventInstance* event = nullptr;
	};

	/*struct AudioListenerComponent
	{
		FMOD_3D_ATTRIBUTES attributes;
	};*/
} // namespace Frac
