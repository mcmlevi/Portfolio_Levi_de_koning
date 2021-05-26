#pragma once
#include "Shape.h"
class Triangle: public Shape
{
public:
	Triangle(const Vec3f& vert0, const Vec3f& vert1, const Vec3f& vert2, const Color& color, const Material& mat);
	~Triangle();
	bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit = nullptr) override;
	Vec3f calculateNormal(const Vec3f& hit) override;
	AABBox* getBoundingVolume() override;
	Color getColor(Vec3f& intersectionPoint);
	Vec3f getCenter() override;
private:
	Vec3f m_vert0;
	Vec3f m_vert1;
	Vec3f m_vert2;
};

