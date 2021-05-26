#include "AABB.h"
#include "Utility.h"

#include "Shape.h"


AABBox::AABBox(const Vec3f& a, const Vec3f b) :bounds{ a.m_x,a.m_y,a.m_z,b.m_x,b.m_y,b.m_z }
{
	if (bounds[0] > bounds[3])
		swap(bounds[0], bounds[3]);
	if (bounds[1] > bounds[4])
		swap(bounds[1], bounds[4]);
	if (bounds[2] < bounds[5])
		swap(bounds[2], bounds[5]);
}

bool AABBox::BoundingVolumeTest(const Ray& ray, float& t, bool insideCheck, float* t2)
{

	float divX{ 1.f / ray.m_direction.m_x };
	float divY{ 1.f / ray.m_direction.m_y };
	float divZ{ 1.f / ray.m_direction.m_z };
	int sign[3]{};
	sign[0] = (divX < 0);
	sign[1] = (divY < 0);
	sign[2] = (divZ < 0);
	Vec3f points[2]{ {bounds[0],bounds[1],bounds[2]},{bounds[3],bounds[4],bounds[5]} };

	float dMinX{};
	float dMaxX{};
	float dMinY{};
	float dMaxY{};
	float dMinZ{};
	float dMaxZ{};

	dMinX = (points[sign[0]].m_x - ray.m_orgin.m_x) * divX;
	dMaxX = (points[1 - sign[0]].m_x - ray.m_orgin.m_x) * divX;
	if (dMinX > dMaxX)
		swap(dMinX, dMaxX);

	dMinY = (points[sign[1]].m_y - ray.m_orgin.m_y) * divY;
	dMaxY = (points[1 - sign[1]].m_y - ray.m_orgin.m_y) * divY;

	if (dMinY > dMaxY) swap(dMinY, dMaxY);

	if (dMinX > dMaxY || dMinY > dMaxX)
		return false;

	dMinZ = (points[sign[2]].m_z - ray.m_orgin.m_z) * divZ;
	dMaxZ = (points[1 - sign[2]].m_z - ray.m_orgin.m_z) * divZ;

	if (dMinZ > dMaxZ) swap(dMinZ, dMaxZ);

	float dMinXY{ dMinY > dMinX ? dMinY : dMinX };
	float dMaxXY{ dMaxY < dMaxX ? dMaxY : dMaxX };

	if (dMinXY > dMaxZ || dMinZ > dMaxXY)
		return false;

	float dMin{ dMinZ > dMinXY ? dMinZ : dMinXY };
	float dMax{ dMaxZ < dMaxXY ? dMaxZ : dMaxXY };
	if (dMin > dMax) swap(dMin, dMax);


	
	if (dMin < 0 && dMax >= 0 && insideCheck)
	{
		t = dMin;
		return true;
	}

	if (dMin < 0)
	{
		dMin = dMax;
		if (dMin < 0)
			return false;
	}

	t = dMin;
	if (t2 != nullptr)
		*t2 = dMax;
	return true;
}

bool AABBox::overlap(AABBox& b)
{
	return { (bounds[0] <= b.bounds[3] && bounds[3] >= bounds[0]) &&
		(bounds[1] <= b.bounds[4] && bounds[4] >= bounds[1]) &&
		(bounds[2] >= b.bounds[5] && bounds[5] <= bounds[2])
	};
}

bool AABBox::inside(AABBox& b)
{
	return (b.bounds[0] >= bounds[0] && b.bounds[1] >= bounds[1] && b.bounds[2] <= bounds[2] && b.bounds[3] <= bounds[3] && b.bounds[4] <= bounds[4] && b.bounds[5] >= bounds[5]);
	
}

bool AABBox::simpleBVTest(const Ray& ray, float& t, bool insideCheck, float* t2)
{
	float divX{ 1.f / ray.m_direction.m_x };
float divY{ 1.f / ray.m_direction.m_y };
float divZ{ 1.f / ray.m_direction.m_z };
int sign[3]{};
sign[0] = (divX < 0);
sign[1] = (divY < 0);
sign[2] = (divZ < 0);
Vec3f points[2]{ {bounds[0],bounds[1],bounds[2]},{bounds[3],bounds[4],bounds[5]} };

float dMinX{};
float dMaxX{};
float dMinY{};
float dMaxY{};
float dMinZ{};
float dMaxZ{};

dMinX = (points[sign[0]].m_x - ray.m_orgin.m_x) * divX;
dMaxX = (points[1 - sign[0]].m_x - ray.m_orgin.m_x) * divX;
if (dMinX > dMaxX)
	swap(dMinX, dMaxX);

dMinY = (points[sign[1]].m_y - ray.m_orgin.m_y) * divY;
dMaxY = (points[1 - sign[1]].m_y - ray.m_orgin.m_y) * divY;

if (dMinY > dMaxY) swap(dMinY, dMaxY);

if (dMinX > dMaxY || dMinY > dMaxX)
	return false;

dMinZ = (points[sign[2]].m_z - ray.m_orgin.m_z) * divZ;
dMaxZ = (points[1 - sign[2]].m_z - ray.m_orgin.m_z) * divZ;

if (dMinZ > dMaxZ) swap(dMinZ, dMaxZ);

float dMinXY{ dMinY > dMinX ? dMinY : dMinX };
float dMaxXY{ dMaxY < dMaxX ? dMaxY : dMaxX };

if (dMinXY > dMaxZ || dMinZ > dMaxXY)
	return false;

float dMin{ dMinZ > dMinXY ? dMinZ : dMinXY };
float dMax{ dMaxZ < dMaxXY ? dMaxZ : dMaxXY };
if (dMin > dMax) swap(dMin, dMax);
if (dMin < 0 && dMax >= 0 && insideCheck)
{
	t = dMin;
	return true;
}

if (dMin < 0)
{
	dMin = dMax;
	if (dMin < 0)
		return false;
}

t = dMin;
if (t2 != nullptr)
*t2 = dMax;
return true;
}

bool AABBox::GetEdges(const Ray& ray)
{
	float t{};
	float t2{};
	
	if (simpleBVTest(ray, t, false, &t2) == false)
		return false;

	if (t == INFINITY || t2 == INFINITY)
		return false;

	//float epsilon{ 0.0001f };
	Vec3f hits[2]
	{
		 ray.m_orgin + ray.m_direction * t,
		 ray.m_orgin + ray.m_direction * t2
	};

	Vec3f m_min{ bounds[0],bounds[1],bounds[2] };
	Vec3f m_max{ bounds[3],bounds[4],bounds[5] };

	
	for (int i = 0; i < 2; ++i)
	{
		 float edgeWidth{ i == 1? 0.05f  * logf(t): 0.05f * logf(t2) };
		 edgeWidth = clamp(edgeWidth, 0.05f, 0.3f);
		// check if it hits[i] the XY plane of the orgin
		if (((hits[i].m_z) - (m_min.m_z)) <= epsilon && ((hits[i].m_z) - (m_min.m_z)) >= -epsilon)
		{
			float minX{ min((m_min.m_x),(m_max.m_x)) };
			float maxX{ max((m_min.m_x),(m_max.m_x)) };

			float minY{ min((m_min.m_y),(m_max.m_y)) };
			float maxY{ max((m_min.m_y),(m_max.m_y)) };

			if ((hits[i].m_x) < minX + edgeWidth || (hits[i].m_x) > maxX - edgeWidth)
				return true;
			if ((hits[i].m_y) < minY + edgeWidth || (hits[i].m_y) > maxY - edgeWidth)
				return true;
		}
		// check if it hits[i] the XY plane of point B
		else if (((hits[i].m_z) - (m_max.m_z)) <= epsilon && ((hits[i].m_z) - (m_max.m_z)) >= -epsilon)
		{
			float minX{ min((m_min.m_x),(m_max.m_x)) };
			float maxX{ max((m_min.m_x),(m_max.m_x)) };

			float minY{ min((m_min.m_y),(m_max.m_y)) };
			float maxY{ max((m_min.m_y),(m_max.m_y)) };

			if ((hits[i].m_x) < minX + edgeWidth || (hits[i].m_x) > maxX - edgeWidth)
				return true;
			if ((hits[i].m_y) < minY + edgeWidth || (hits[i].m_y) > maxY - edgeWidth)
				return true;
		}
		// Check the YZ plane of the origin
		else if (((hits[i].m_x) - (m_min.m_x)) <= epsilon && ((hits[i].m_x) - (m_min.m_x)) >= -epsilon)
		{
			if ((hits[i].m_z) > (m_min.m_z) + edgeWidth || (hits[i].m_z) < (m_max.m_z) - edgeWidth)
				return true;
			if ((hits[i].m_y) < (m_min.m_y) + edgeWidth || (hits[i].m_y) > (m_max.m_y) - edgeWidth)
				return true;
		}
		// Check the YZ plane of point B
		else if (((hits[i].m_x) - (m_max.m_x)) <= epsilon && ((hits[i].m_x) - (m_max.m_x)) >= -epsilon)
		{
			if ((hits[i].m_z) > (m_min.m_z) + edgeWidth || (hits[i].m_z) < (m_max.m_z) - edgeWidth)
				return true;
			if ((hits[i].m_y) < (m_min.m_y) + edgeWidth || (hits[i].m_y) > (m_max.m_y) - edgeWidth)
				return true;
		}
		// Check the XZ plane of the orgin
		else if (((hits[i].m_y) - (m_min.m_y)) <= epsilon && ((hits[i].m_y) - (m_min.m_y)) >= -epsilon)
		{
			if ((hits[i].m_z) > (m_min.m_z) + edgeWidth || (hits[i].m_z) < (m_max.m_z) - edgeWidth)
				return true;
			if ((hits[i].m_x) < (m_min.m_x) + edgeWidth || (hits[i].m_x) > (m_max.m_x) - edgeWidth)
				return true;
		}
		// Check the XZ plane of point B
		else if (((hits[i].m_y) - (m_max.m_y)) <= epsilon && ((hits[i].m_y) - (m_max.m_y)) >= -epsilon)
		{
			if ((hits[i].m_z) > (m_min.m_z) + edgeWidth || (hits[i].m_z) < (m_max.m_z) - edgeWidth)
				return true;
			if ((hits[i].m_x) < (m_min.m_x) + edgeWidth || (hits[i].m_x) > (m_max.m_x) - edgeWidth)
				return true;
		}
	}

	return false;
}

Vec3f AABBox::getCenter()
{
	Vec3f a{ bounds[0],bounds[1],bounds[2] };
	Vec3f b{ bounds[3],bounds[4],bounds[5] };
	return a + ((b - a) * 0.5f);
}