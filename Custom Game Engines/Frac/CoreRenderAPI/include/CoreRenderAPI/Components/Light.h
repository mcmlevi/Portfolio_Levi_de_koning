#pragma once
#include "glm/vec3.hpp"

namespace TOR 
{
	/// <summary>
	/// The light represents a directional light by default, though the direction is passed in through the use of the worldmatrix.
	/// </summary>
	struct DirectionalLight
	{
		glm::vec3 Color;
		float Intensity;
	};

	/// <summary>
	/// The point light represents a light source that shines in all directions constraint by range.
	/// </summary>
	struct PointLight
	{
		glm::vec3 Color;
		float Intensity;
		float Radius;
	};

	/// <summary>
	/// A spot light defines a light with a direction and a specified angle within the inner and outer cone.
	/// *note that the direction is derived from the rotation of the world matrix.
	/// </summary>
	struct SpotLight
	{
		glm::vec3 Color;
		float Intensity;
		float Radius;
		float InnerCone;
		float OuterCone;
	};
}