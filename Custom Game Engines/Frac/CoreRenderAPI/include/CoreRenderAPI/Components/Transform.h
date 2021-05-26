#pragma once
#include <Glm/vec3.hpp>
#include <Glm/mat4x4.hpp>
#include <Glm/gtx/quaternion.hpp>

namespace TOR
{
	struct Transform
	{
		glm::vec3 Position = { 0,0,0 };
		glm::vec3 Scale = { 1,1,1 };
		glm::quat Orientation = glm::identity<glm::quat>();
		glm::vec3 EulerOrientation = { 0,0,0 };
		glm::mat4x4 ModelTransformData = glm::identity<glm::mat4x4>();
		bool isDirty = true;
		bool isStationary = false;
	};
}
