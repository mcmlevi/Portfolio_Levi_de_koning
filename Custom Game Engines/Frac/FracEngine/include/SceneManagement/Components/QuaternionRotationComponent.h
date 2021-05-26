
#pragma once
#include "glm/gtx/quaternion.hpp"
namespace Frac
{
	struct QuaternionRotationComponent
	{
	public:
		union
		{
			glm::quat quaternionRotationData = glm::quat{glm::vec3{0.f,0.f,0.f}};
			struct
			{
				float x, y, z, w;
			};
		};
	};
}