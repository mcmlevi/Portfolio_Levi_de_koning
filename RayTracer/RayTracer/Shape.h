#pragma once
#include "vec3f.h"
#include "Ray.h"
#include "Color.h"
#include "material.h"
#include "Transform.h"
#include "Mat4x4.h"

class AABBox;
class Shape
{
public:
	Shape(Vec3f orgin, Color color,Material mat);
	virtual ~Shape() = default;
	Vec3f m_orgin;
	Vec3f m_normalVector;
	Material m_mat;
	virtual bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit = nullptr) = 0;
	virtual Vec3f calculateNormal(const Vec3f& hit) = 0;
	virtual AABBox* getBoundingVolume() = 0;
	virtual Color getColor(Vec3f& intersectionPoint) = 0;
	virtual Vec3f getCenter() = 0;
	
	void rotateX(const float& degrees);
	void rotateY(const float& degrees);
	void rotateZ(const float& degrees);
	Color getUVColor(float u, float v, const Image* image);
	Vec3f calculateBumpNormal(float u, float v, Vec3f& worldNormal);
	Transform transform;
protected:
	Color m_color;
	

};


