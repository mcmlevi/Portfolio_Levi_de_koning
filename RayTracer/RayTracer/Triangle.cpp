#include "Triangle.h"
#include "Utility.h"
#include "AABB.h"

Triangle::Triangle(const Vec3f& vert0, const Vec3f& vert1, const Vec3f& vert2, const Color& color, const Material& mat) :Shape(vert0, color, mat),m_vert0(vert0),m_vert1(vert1),m_vert2(vert2)
{
	Vec3f v{ vert1 - vert0 };
	Vec3f w{  vert2  - vert0};

	m_normalVector = v.cross(w);
	//m_normalVector {};

	
	m_normalVector.normalize();
}

Triangle::~Triangle()
{
	
}

#define UNUSED(x) (void)x
bool Triangle::IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit)
{
	static_cast<void>(backHit);
	shape = this;
	float t{};
	float u{}, v{};
	//Vec3f localRayOrigin{ ray.m_orgin + ray.m_orgin * inverse.getTranslation() };
	Vec3f localRayOrigin{ (transform.getInverseMatrix() * Vec4f{ray.m_orgin,1.f}).m_XYZ };
	Vec3f localRayDirection{ (transform.getInverseMatrix() * Vec4f{ray.m_direction,0.f}).m_XYZ.normalize() };

	Vec3f v0v1 = m_vert1 - m_vert0;
	Vec3f v0v2 = m_vert2 - m_vert0;
	Vec3f pvec = localRayDirection.cross(v0v2);
	float det = v0v1.dot(pvec);
	
	//if (det < epsilon) return false;
	if (fabs(det) < epsilon) return false;

	float invDet = 1 / det;

	Vec3f tvec = localRayOrigin - m_vert0 ;
	u = tvec.dot(pvec) * invDet;
	if (u < 0 || u > 1) return false;

	Vec3f qvec = tvec.cross(v0v1);
	v = localRayDirection.dot(qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	t = v0v2.dot(qvec) * invDet;
	if (t < 0)
		return  false;
	
	
	hit = (transform.getLocalMatrix() * Vec4f{ localRayOrigin + (localRayDirection * t),1.f }).m_XYZ;

	return true;

}

Vec3f Triangle::calculateNormal(const Vec3f& hit)
{
	(void)hit;
	Mat4x4 inverseCopy{ transform.getInverseMatrix() };
	inverseCopy.transpose();
	//Vec3f normalInLocalSpace{ (transform.getInverseMatrix() * Vec4f{m_normalVector,0.f}).m_XYZ };
	Vec4f transformedNormal{ inverseCopy * Vec4f { m_normalVector, 0.f } };
	return transformedNormal.m_XYZ.normalize();
	//return  { (localMatrix *  transformedNormal).m_XYZ.normalize() };
}

AABBox* Triangle::getBoundingVolume()
{
	
	
	Vec3f verts[3]
	{
		{ (transform.getLocalMatrix() * Vec4f {m_vert0,1.f}).m_XYZ },{ (transform.getLocalMatrix() * Vec4f {m_vert1,1.f}).m_XYZ },{ (transform.getLocalMatrix() * Vec4f {m_vert2,1.f}).m_XYZ }
	};

	Vec3f min{ verts[0] };
	Vec3f max{ verts[0] };


	for (int i = 0; i < 3; ++i)
	{
		if (verts[i].m_x <= min.m_x)
			min.m_x = verts[i].m_x;

		if (verts[i].m_x >= max.m_x)
			max.m_x = verts[i].m_x;

		if (verts[i].m_y <= min.m_y)
			min.m_y = verts[i].m_y;

		if (verts[i].m_y >= max.m_y)
			max.m_y = verts[i].m_y;

		if (verts[i].m_z >= min.m_z)
			min.m_z = verts[i].m_z;

		if (verts[i].m_z <= max.m_z)
			max.m_z = verts[i].m_z;
	}




	return new AABBox{ min,max };
}

Color Triangle::getColor(Vec3f& intersectionPoint)
{
	(void)intersectionPoint;
	return m_color;
}

Vec3f Triangle::getCenter()
{
	return Vec3f{};
}
