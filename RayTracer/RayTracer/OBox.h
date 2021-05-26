#pragma once
#include "Shape.h"
class OBox: public Shape
{
	public:
	OBox(const Vec3f& orgin, const Vec3f& b, const Color& color, const Material& mat);
	virtual AABBox* getBoundingVolume() override;
	virtual bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit = nullptr) override;
	virtual Vec3f calculateNormal(const Vec3f& hit) override;
	virtual Color getColor(Vec3f& intersectionPoint) override;
	Vec3f getCenter() override;
	void getUV(float& u, float& v, Vec3f& localIntersect);

	
	Vec3f m_B;
private:
	Vec3f* bounds[2];
};

inline Vec3f OBox::getCenter()
{
	return (transform.getLocalMatrix() * Vec4f{ m_orgin + ((m_B - m_orgin) * 0.5f) ,1.f}).m_XYZ;
}

