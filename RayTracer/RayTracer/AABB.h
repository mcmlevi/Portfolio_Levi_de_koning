#pragma once
#include "Shape.h"

class AABBox
{
public:
	AABBox(const Vec3f& orgin, const Vec3f b);
	AABBox() = default;
	~AABBox() = default;
	bool BoundingVolumeTest(const Ray& ray, float& t, bool insideCheck = true, float* back = nullptr);

	bool GetEdges(const Ray& ray);
	Vec3f getCenter();
	float bounds[6];
	bool overlap(AABBox& b);
	bool inside(AABBox& b);
private:
	bool  simpleBVTest(const Ray& ray, float& t, bool insideCheck = true, float* back = nullptr);
};
