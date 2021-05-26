#pragma once
#include "shape.h"
#include "Triangle.h"
#include "AABB.h"
#include "vector.h"

enum class TriangleType
{
	SIMPLE,
	COMPLEX,
	NO_TEXTURE,
	COMPLEX_QUAD,
};
class Mesh: public Shape
{
public:
	Mesh(const char* src, TriangleType type, Color& color, Material& mat);
	~Mesh() override;

	bool IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit) override;
	AABBox* getBoundingVolume() override;
	Color getColor(Vec3f& intersectionPoint) override;
	Vec3f getCenter() override;
	Vec3f calculateNormal(const Vec3f& hit) override;
	ldk::Vector<Triangle*> triangles;
private:
	ldk::Vector<unsigned int> vertexIndexes, normalIndexes;
	ldk::Vector<Vec3f*> vertices;
	ldk::Vector<Vec3f*> normals;
	

	AABBox updateBounds();
	
	AABBox _bounds{};
};

