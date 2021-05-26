#include "Sphere.h"
#include "Utility.h"
#include "AABB.h"

Sphere::Sphere(const Vec3f& orgin, float radius, const Color& color, const Material& mat) :Shape(orgin, color, mat), m_radius(radius)
{
	m_radius2 = m_radius * m_radius;
}

bool Sphere::IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit)
{
	shape = this;
	//Vec3f localRayOrigin{ ray.m_orgin + ray.m_orgin * inverse.getTranslation() };
	Vec3f localRayOrigin{ (transform.getInverseMatrix() * Vec4f{ray.m_orgin,1.f}).m_XYZ };
	Vec3f localRayDirection{ (transform.getInverseMatrix() * Vec4f{ray.m_direction,0.f}).m_XYZ.normalize() };
	float p1{}, p2{};

	if ((m_orgin - localRayOrigin).lenght2() <= m_radius2)
	{
		// geometric solution
		Vec3f L = m_orgin - localRayOrigin;
		float tca = L.dot(localRayDirection);
		// if (tca < 0) return false;
		float d2 = L.dot(L) - tca * tca;
		if (d2 > m_radius2) return false;
		float thc = sqrt(m_radius2 - d2);
		p1 = tca - thc;
		p2 = tca + thc;
	}

	else
	{
		Vec3f distance{ (localRayOrigin - m_orgin) };
		//float a{ localRayDirection.dot(localRayDirection) };
		float a{ 1.f };
		float b{ 2 * (localRayDirection.dot(distance)) };

		float c{ (distance.dot(distance)) - m_radius2 };
		if (solveQuadratic(a, b, c, p1, p2) == false)
			return  false;
	}

	if (p1 > p2)swap(p1, p2);
	if (p1 < 0)
	{
		p1 = p2;
		if (p1 < 0) return  false;
	}

	hit = (transform.getLocalMatrix() * Vec4f{ localRayOrigin + (localRayDirection * p1),1.f }).m_XYZ;

	if (backHit != nullptr)
		*backHit = (transform.getLocalMatrix() * Vec4f{ localRayOrigin + (localRayDirection * p2),1.f }).m_XYZ;

	return true;
}

AABBox* Sphere::getBoundingVolume()
{
	Vec3f worldOrigin{ (transform.getLocalMatrix() * Vec4f {m_orgin,1.f}).m_XYZ };
	float rX{m_radius * transform.getLocalMatrix().m_X.m_XYZ.lenght()};
	float rY{ m_radius * transform.getLocalMatrix().m_Y.m_XYZ.lenght() };
	float rZ{ m_radius * transform.getLocalMatrix().m_Z.m_XYZ.lenght() };
	Vec3f corners[8]
	{
		//front bottom left
		{  worldOrigin.m_x - rX,worldOrigin.m_y - rY,worldOrigin.m_z + rZ},
		// front bottom right
		{  worldOrigin.m_x + rX,worldOrigin.m_y - rY,worldOrigin.m_z + rZ},
		// front top left
		{  worldOrigin.m_x - rX,worldOrigin.m_y + rY,worldOrigin.m_z + rZ},
		// front top right
		{  worldOrigin.m_x + rX,worldOrigin.m_y + rY,worldOrigin.m_z + rZ},
		// back top right
		{  worldOrigin.m_x + rX,worldOrigin.m_y + rY,worldOrigin.m_z - rZ},
		// back top left
		{  worldOrigin.m_x - rX,worldOrigin.m_y + rY,worldOrigin.m_z - rZ},
		// back bottom left
		{  worldOrigin.m_x - rX,worldOrigin.m_y - rY,worldOrigin.m_z - rZ},
		// back bottom right
		{ worldOrigin.m_x + rX,worldOrigin.m_y - rY,worldOrigin.m_z - rZ}
	};

	Vec3f min{ corners[0] };
	Vec3f max{ corners[0] };


	for (int i = 0; i < 8; ++i)
	{
		if (corners[i].m_x <= min.m_x)
			min.m_x = corners[i].m_x;

		if (corners[i].m_x >= max.m_x)
			max.m_x = corners[i].m_x;

		if (corners[i].m_y <= min.m_y)
			min.m_y = corners[i].m_y;

		if (corners[i].m_y >= max.m_y)
			max.m_y = corners[i].m_y;

		if (corners[i].m_z >= min.m_z)
			min.m_z = corners[i].m_z;

		if (corners[i].m_z <= max.m_z)
			max.m_z = corners[i].m_z;
	}


	
	
	return new AABBox{ min,max };
}

Vec3f Sphere::calculateNormal(const Vec3f& hit)
{
	Vec3f localIntersect{ (transform.getInverseMatrix() * Vec4f{hit,1.f}).m_XYZ };
	if (m_mat.bumpMap == nullptr) {

		Mat4x4 inverseCopy{ transform.getInverseMatrix() };
		inverseCopy.transpose();
		return { (inverseCopy * Vec4f{localIntersect,0.f}).m_XYZ.normalize() };
		//return ( (localMatrix * transformedNormal)).m_XYZ.normalize();
	}
	else
	{
		Vec3f worldNormal{ (transform.getLocalMatrix() * Vec4f{ (localIntersect - m_orgin),0.f }).m_XYZ.normalize() };
		//Vec3f localIntersect{ (inverseLocalMatrix * Vec4f{hit,1.f}).m_XYZ };
		Vec3f normal{ localIntersect - m_orgin };
		float u{ atan2f(normal.m_x,normal.m_z) / (2 * M_PI) + 0.5f };
		float v{ normal.m_y * 0.5f + 0.5f };
		return calculateBumpNormal(u, v, worldNormal);
		
	}
}

Color Sphere::getColor(Vec3f& intersectionPoint)
{
	
	if(m_mat.texture != nullptr)
	{
		Vec3f localIntersect{ (transform.getInverseMatrix() * Vec4f{intersectionPoint,1.f}).m_XYZ };
		Vec3f normal{ localIntersect - m_orgin };
		normal.normalize();
		float u{ atan2f(normal.m_x,normal.m_z) / (2 * M_PI) + 0.5f };
		float v{ normal.m_y * 0.5f + 0.5f };
		return getUVColor(u, v, m_mat.texture);
	}
	else
		return m_color;
}

Vec3f Sphere::getCenter()
{
	 return { (transform.getInverseMatrix() * Vec4f{m_orgin,1.f}).m_XYZ };
}
