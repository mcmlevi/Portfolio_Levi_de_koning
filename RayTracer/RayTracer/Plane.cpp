#include"Plane.h"
#include "Utility.h"
#include <cassert>

Plane::Plane(Vec3f orgin, Color color, Material mat, Color tile2, Vec3f min, Vec3f max) :Shape(orgin, color,mat),checker2ndColor(tile2),m_min(min),m_max(max)
{
	m_normalVector = Vec3f{ 1.f,0,1.f }.cross({ 1.f,0,0.f });
}

bool Plane::IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit)
{
	shape = this;
	assert(m_orgin != ray.m_orgin);
	float demoninator = -1 * (m_normalVector).dot(ray.m_direction);
	if (demoninator > epsilon)
	{
		float d = ( ( ray.m_orgin - m_orgin).dot(m_normalVector)) / demoninator;
		if (d >= epsilon)
		{
			hit = ray.m_orgin + ray.m_direction * d;

			if(!(hit.m_x >= m_min.m_x && hit.m_x <= m_max.m_x && hit.m_y >= m_min.m_y && hit.m_y <= m_max.m_y && hit.m_z <= m_min.m_z && hit.m_z >= m_max.m_z))
				return false;

			if (backHit != nullptr)
				*backHit = ray.m_orgin + ray.m_direction * d - m_normalVector * 1e-4f;
			
			//m_color.reflectiveIndex = 0.3f;
			return true;
		}
	}
	return  false;
}

AABBox* Plane::getBoundingVolume()
{
	return nullptr;
}

Vec3f Plane::calculateNormal(const Vec3f& hit)
{
	static_cast<void>(hit);
	return  m_normalVector;
}

Color Plane::getColor(Vec3f& intersectionPoint)
{
	if (checkers) 
	{
		if ((static_cast<int>(floor(intersectionPoint.m_x)) + static_cast<int>(floor(intersectionPoint.m_z))) % 2 == 0)
		{
			return m_color;
			//return  Color{ 130.f / 255.f,1.f,130.f / 255.f };
		}
		else
		{
			return checker2ndColor;
			//return  Color{ 10.f / 255.f,10.f / 255.f,50.f / 255.f };
		}
	}
	else
	{
		return  m_color;
	}
}

Vec3f Plane::getCenter()
{
	return Vec3f{ };
}

