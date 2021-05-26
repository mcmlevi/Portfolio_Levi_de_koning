#include "pch.h"
#include "Ray.h"

Ray::Ray() : m_Origin(0), m_Direction(glm::vec3(0, 0, 1)), m_Length(1)
{

}

Ray::Ray(glm::vec3 origin, glm::vec3 direction, float length)
{
	m_Origin = origin;
	m_Direction = direction;
	m_Length = length;

	m_EndPoint = m_Origin + m_Direction * m_Length;
}

Ray::Ray(glm::vec3 origin, glm::vec3 endPoint)
{
	m_Origin = origin;
	m_EndPoint = endPoint;

	m_Direction = glm::normalize(m_Origin - m_EndPoint);

	m_Length = glm::length(m_Origin - m_EndPoint);
}