#pragma once
#include "vec3f.h"
#include "Color.h"
struct Light
{
	Light(const Vec3f& lighlocation,const Color& color, float lightIntensity = 1.f, float area = 0.5f):m_lightLocation(lighlocation),m_lightColor(color),m_lightIntensity(lightIntensity),m_area(area){}
	Vec3f m_lightLocation{};
	Color m_lightColor{};
	float m_lightIntensity{};
	float m_area{};
	
};
