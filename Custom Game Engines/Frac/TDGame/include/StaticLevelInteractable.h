#pragma once
#include <string>
#include "Glm/gtx/transform.hpp"
struct StaticLevelInteractable
{
public:
	int m_objectId;
	std::string m_name;
	glm::vec3 m_position;
	glm::vec3 m_rotation;
private:
};