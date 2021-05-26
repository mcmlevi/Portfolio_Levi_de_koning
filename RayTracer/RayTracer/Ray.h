#pragma once
#include "vec3f.h"
#include "Color.h"

class Ray
{
public:
	Ray(Vec3f direction, Vec3f orgin, Color color = {0.f,0.f,0.f});

	Vec3f m_direction;
	Vec3f m_orgin;
	Color m_color;

};

