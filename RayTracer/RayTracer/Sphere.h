#pragma once
#include "shape.h"
#include "Mat4x4.h"
class Sphere : public Shape
{
public:
	Sphere(const Vec3f& orgin, float radius, const Color& color, const Material& mat);
	bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit = nullptr) override;
	AABBox* getBoundingVolume() override;
	float m_radius;
	Vec3f calculateNormal(const Vec3f& hit) override;
	Color getColor(Vec3f& intersectionPoint) override;
	Vec3f getCenter() override;
private:
	float m_radius2;

};
