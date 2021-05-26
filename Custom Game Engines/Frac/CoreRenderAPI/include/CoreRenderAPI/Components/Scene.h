#pragma once
#include "Glm/vec3.hpp"

namespace TOR 
{
	struct Scene 
	{
		glm::vec3 AmbientLightColor;
		float AmbientLightFactor;
	};
}