#pragma once
#include <glm/glm.hpp>


class Ray
{
public:
	Ray();
	Ray(glm::vec3 origin, glm::vec3 direction, float length);
	Ray(glm::vec3 origin, glm::vec3 endPoint);
	~Ray() = default;

	
	glm::vec3 m_Origin;
	glm::vec3 m_Direction;
	glm::vec3 m_EndPoint;
	float m_Length;
};

