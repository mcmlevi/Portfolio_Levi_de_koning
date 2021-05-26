
#pragma once
#include "glm/vec3.hpp"
namespace Frac
{
	struct PositionComponent
	{
	public:
		union
		{
			glm::vec3 position;
			struct
			{
				float x, y, z;
			};
		};
	};
}
