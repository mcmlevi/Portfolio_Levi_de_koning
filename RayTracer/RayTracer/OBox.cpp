#include "OBox.h"
#include "Utility.h"
#include "AABB.h"
OBox::OBox(const Vec3f& orgin, const Vec3f& b, const Color& color, const Material& mat) : Shape(orgin, color, mat), m_B(b)
{
	bounds[0] = &m_orgin;
	bounds[1] = &m_B;
}

AABBox* OBox::getBoundingVolume()
{
	Vec3f lenghts{ 1.f,1.f,-1.f };
	Vec3f corners[8]
	{
		m_orgin,
		Vec3f{m_orgin.m_x + lenghts.m_x,	m_orgin.m_y,					m_orgin.m_z},
		Vec3f{m_orgin.m_x				, m_orgin.m_y + lenghts.m_y,		m_orgin.m_z},
		Vec3f{m_orgin.m_x + lenghts.m_x ,m_orgin.m_y + lenghts.m_y,		m_orgin.m_z},
		m_B,
		Vec3f{m_B.m_x - lenghts.m_x,m_B.m_y,m_B.m_z},
		Vec3f{m_B.m_x ,m_B.m_y - lenghts.m_y,m_B.m_z},
		Vec3f{m_B.m_x - lenghts.m_x ,m_B.m_y - lenghts.m_y,m_B.m_z},
	};

	for (int i = 0; i < 8; ++i)
	{
		corners[i] = (transform.getLocalMatrix() * Vec4f{ corners[i],1.f }).m_XYZ;
	}

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

	AABBox* box = new AABBox(min, max);
	return box;
}
void OBox::getUV(float& u, float& v, Vec3f& localIntersect)
{
	if (((localIntersect.m_z) - (m_orgin.m_z)) <= epsilon && ((localIntersect.m_z) - (m_orgin.m_z)) >= -epsilon)
	{
		u = localIntersect.m_x + 0.5f;
		v = localIntersect.m_y + 0.5f;
	}
	// check if it localIntersect the XY plane of point B
	else if (((localIntersect.m_z) - (m_B.m_z)) <= epsilon && ((localIntersect.m_z) - (m_B.m_z)) >= -epsilon)
	{
		u = localIntersect.m_x + 0.5f;
		v = localIntersect.m_y + 0.5f;
	}
	// Check the YZ plane of the origin
	else if (((localIntersect.m_x) - (m_orgin.m_x)) <= epsilon && ((localIntersect.m_x) - (m_orgin.m_x)) >= -epsilon)
	{
		u = localIntersect.m_z + 0.5f;
		v = localIntersect.m_y + 0.5f;
	}
	// Check the YZ plane of point B
	else if (((localIntersect.m_x) - (m_B.m_x)) <= epsilon && ((localIntersect.m_x) - (m_B.m_x)) >= -epsilon)
	{
		u = localIntersect.m_z + 0.5f;
		v = localIntersect.m_y + 0.5f;
	}
	// Check the XZ plane of the orgin
	else if (((localIntersect.m_y) - (m_orgin.m_y)) <= epsilon && ((localIntersect.m_y) - (m_orgin.m_y)) >= -epsilon)
	{
		u = localIntersect.m_x + 0.5f;
		v = localIntersect.m_z + 0.5f;
	}
	// Check the XZ plane of point B
	else if (((localIntersect.m_y) - (m_B.m_y)) <= epsilon && ((localIntersect.m_y) - (m_B.m_y)) >= -epsilon)
	{
		u = localIntersect.m_x + 0.5f;
		v = localIntersect.m_z + 0.5f;
	}
}

bool OBox::IntersectionTest(const Ray& ray, Vec3f& hit,Shape*& shape, Vec3f* backHit)
{
	shape = this;
	Vec3f localRayOrigin{ (transform.getInverseMatrix() * Vec4f{ray.m_orgin,1.f}).m_XYZ };
	Vec3f localRayDirection{ (transform.getInverseMatrix() * Vec4f{ray.m_direction,0.f}).m_XYZ };

	float divX{ 1.f / localRayDirection.m_x };
	float divY{ 1.f / localRayDirection.m_y };
	float divZ{ 1.f / localRayDirection.m_z };
	int sign[3]{};
	sign[0] = (divX < 0);
	sign[1] = (divY < 0);
	sign[2] = (divZ < 0);

	float dMinX{};
	float dMaxX{};
	float dMinY{};
	float dMaxY{};
	float dMinZ{};
	float dMaxZ{};

	dMinX = (bounds[sign[0]]->m_x - localRayOrigin.m_x) * divX;
	dMaxX = (bounds[1 - sign[0]]->m_x - localRayOrigin.m_x) * divX;
	if (dMinX > dMaxX)
		swap(dMinX, dMaxX);

	dMinY = (bounds[sign[1]]->m_y - localRayOrigin.m_y) * divY;
	dMaxY = (bounds[1 - sign[1]]->m_y - localRayOrigin.m_y) * divY;

	if (dMinY > dMaxY) swap(dMinY, dMaxY);

	if (dMinX > dMaxY || dMinY > dMaxX)
		return false;

	dMinZ = (bounds[sign[2]]->m_z - localRayOrigin.m_z) * divZ;
	dMaxZ = (bounds[1 - sign[2]]->m_z - localRayOrigin.m_z) * divZ;

	if (dMinZ > dMaxZ) swap(dMinZ, dMaxZ);

	float dMinXY{ dMinY > dMinX ? dMinY : dMinX };
	float dMaxXY{ dMaxY < dMaxX ? dMaxY : dMaxX };

	if (dMinXY > dMaxZ || dMinZ > dMaxXY)
		return false;

	float dMin{ dMinZ > dMinXY ? dMinZ : dMinXY };
	float dMax{ dMaxZ < dMaxXY ? dMaxZ : dMaxXY };
	if (dMin > dMax) swap(dMin, dMax);

	if (dMin < 0)
	{
		dMin = dMax;
		if (dMin < 0)
			return false;
	}

	hit = (transform.getLocalMatrix() * Vec4f{ localRayOrigin + (localRayDirection * dMin),1.f }).m_XYZ;
	//m_normalVector = calculateNormal(hit);
	if (backHit != nullptr)
		*backHit = (transform.getLocalMatrix() * Vec4f{ localRayOrigin + (localRayDirection * dMax),1.f }).m_XYZ;

	return true;
}

Vec3f OBox::calculateNormal(const Vec3f& hit)
{
	Vec3f localIntersect{ (transform.getInverseMatrix() * Vec4f{hit,1.f}).m_XYZ };
	Vec3f normalVec{};

	if (((localIntersect.m_z) - (m_orgin.m_z)) <= epsilon && ((localIntersect.m_z) - (m_orgin.m_z)) >= -epsilon)
	{
		normalVec = { 0,0,1 };
	}
	// check if it localIntersect the XY plane of point B
	else if (((localIntersect.m_z) - (m_B.m_z)) <= epsilon && ((localIntersect.m_z) - (m_B.m_z)) >= -epsilon)
	{
		normalVec = { 0,0,-1 };
	}
	// Check the YZ plane of the origin
	else if (((localIntersect.m_x) - (m_orgin.m_x)) <= epsilon && ((localIntersect.m_x) - (m_orgin.m_x)) >= -epsilon)
	{
		normalVec = { -1,0,0 };
	}
	// Check the YZ plane of point B
	else if (((localIntersect.m_x) - (m_B.m_x)) <= epsilon && ((localIntersect.m_x) - (m_B.m_x)) >= -epsilon)
	{
		normalVec = { 1,0,0 };
	}
	// Check the XZ plane of the orgin
	else if (((localIntersect.m_y) - (m_orgin.m_y)) <= epsilon && ((localIntersect.m_y) - (m_orgin.m_y)) >= -epsilon)
	{
		normalVec = { 0,-1,0 };
	}
	// Check the XZ plane of point B
	else if (((localIntersect.m_y) - (m_B.m_y)) <= epsilon && ((localIntersect.m_y) - (m_B.m_y)) >= -epsilon)
	{
		normalVec = { 0,1,0 };
	}
	if(m_mat.bumpMap == nullptr)
		return (transform.getLocalMatrix() * Vec4f{ normalVec,0.f }).m_XYZ.normalize();
	else
	{
		Vec3f worldNormal{ (transform.getLocalMatrix() * Vec4f{ normalVec,0.f }).m_XYZ.normalize() };
		float u{};
		float v{};
		getUV(u, v, localIntersect);
		return  calculateBumpNormal(u, v, worldNormal);
	}
}

Color OBox::getColor(Vec3f& intersectionPoint)
{
	if (m_mat.texture != nullptr)
	{
		Vec3f localIntersect{ (transform.getInverseMatrix() * Vec4f{intersectionPoint,1.f}).m_XYZ };
		
		float u{};
		float v{};
		getUV(u, v, localIntersect);

	
	
		return (getUVColor(u, v, m_mat.texture) * m_color);
	}

	return m_color;
}