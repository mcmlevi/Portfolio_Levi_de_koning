#pragma once
#include"Shape.h"
class  Plane : public Shape
{
public:
	Plane(Vec3f orgin, Color color, Material mat, Color tile2 = Color{0.1f,0.1f,0.1f},Vec3f min = { -INFINITY,-INFINITY,INFINITY }, Vec3f max = { INFINITY,INFINITY,-INFINITY }) ;
	bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit = nullptr) override;
	virtual AABBox* getBoundingVolume() override;
	Vec3f calculateNormal(const Vec3f& hit) override;
	Color getColor(Vec3f& intersectionPoint);
	Vec3f getCenter() override;
private:
	bool checkers{ true };
	Color checker2ndColor;
	Vec3f m_min;
	Vec3f m_max;
};


