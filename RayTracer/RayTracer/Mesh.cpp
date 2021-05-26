#include "Mesh.h"
#pragma warning(push)
#pragma warning(disable: 26451 6054)
Mesh::Mesh(const char* src, TriangleType type,Color& color, Material& mat) :Shape({}, color, mat)
{
	FILE* file;
	fopen_s(&file, src, "r");
	if (file == nullptr)
	{
		printf("File failed to open\n");
		perror("Error");
		return;
	}

	while (true)
	{
		char lineHeader[128];
		int res = fscanf_s(file, "%s", lineHeader, 125);

		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			Vec3f temp;
			fscanf_s(file, "%f %f %f\n", &temp.m_x, &temp.m_y, &temp.m_z);
			vertices.push_back(new Vec3f{temp});
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			Vec3f temp;
			fscanf_s(file, "%f %f %f\n", &temp.m_x, &temp.m_y, &temp.m_z);
			normals.push_back(new Vec3f{temp});
		}
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vert1, vert2, vert3;
			unsigned int vertexIndex[4], uvIndex[4], normalIndex[4];

			int matches{};
			if (type == TriangleType::COMPLEX)
			{
				matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches == 9)
				{
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[1] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);

					normalIndexes.push_back(normalIndex[0] - 1);
					normalIndexes.push_back(normalIndex[1] - 1);
					normalIndexes.push_back(normalIndex[2] - 1);
				}
			}
			
			if (type == TriangleType::COMPLEX_QUAD)
			{
				matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2], &vertexIndex[3], &uvIndex[3], &normalIndex[3]);
				if (matches == 12)
				{
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[1] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);
					
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);
					vertexIndexes.push_back(vertexIndex[3] - 1);

					normalIndexes.push_back(normalIndex[0] - 1);
					normalIndexes.push_back(normalIndex[1] - 1);
					normalIndexes.push_back(normalIndex[2] - 1);

					normalIndexes.push_back(normalIndex[0] - 1);
					normalIndexes.push_back(normalIndex[2] - 1);
					normalIndexes.push_back(normalIndex[3] - 1);

				}
				if (matches == 9)
				{
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[1] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);

					normalIndexes.push_back(normalIndex[0] - 1);
					normalIndexes.push_back(normalIndex[1] - 1);
					normalIndexes.push_back(normalIndex[2] - 1);
				}
			}
			if (type == TriangleType::NO_TEXTURE)
			{
				matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0],  &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
				if (matches == 6)
				{
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[1] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);

					normalIndexes.push_back(normalIndex[0] - 1);
					normalIndexes.push_back(normalIndex[1] - 1);
					normalIndexes.push_back(normalIndex[2] - 1);
				}
			}
			if (type == TriangleType::SIMPLE)
			{
				matches = fscanf_s(file, "%d %d %d\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
				if (matches == 3)
				{
					vertexIndexes.push_back(vertexIndex[0] - 1);
					vertexIndexes.push_back(vertexIndex[1] - 1);
					vertexIndexes.push_back(vertexIndex[2] - 1);
				}
			}
		}
	}

	for (unsigned i = 0; i < vertexIndexes.size(); i += 3)
	{
	
		triangles.push_back(new Triangle{ *vertices[vertexIndexes[i]],*vertices[vertexIndexes[i + 1]],*vertices[vertexIndexes[i + 2]],m_color,m_mat });
		if(i + 2 < normals.size())
		{
			Vec3f normal1{ *normals[normalIndexes[i]] };
			Vec3f normal2{ *normals[normalIndexes[i + 1]] };
			Vec3f normal3{ *normals[normalIndexes[i + 2]] };
			Vec3f normal{ ((normal1 +normal2  + normal3) / 3.f).normalize() };
			(*triangles.back()).m_normalVector = normal;
		}
	}
	_bounds = updateBounds();

	fclose(file);
}

Mesh::~Mesh()
{
	for (int i = 0; i < vertices.size(); ++i)
	{
		delete vertices[i];
		vertices[i] = nullptr;
	}
	for (int i = 0; i < normals.size(); ++i)
	{
		delete normals[i];
		normals[i] = nullptr;
	}
	for (int i = 0; i < triangles.size(); ++i)
	{
		delete triangles[i];
		triangles[i] = nullptr;
	}
}

bool Mesh::IntersectionTest(const Ray& ray, Vec3f& hit, Shape*& shape, Vec3f* backHit)
{
	static_cast<void>(backHit);
	static_cast<void>(ray);
	static_cast<void>(hit);
	static_cast<void>(shape );
	return false;
	//float distance{ INFINITY };


	//float t{};
	//if (_bounds.BoundingVolumeTest(ray, t))
	//{
	//	for (unsigned i = 0; i < triangles.size(); ++i)
	//	{
	//		Vec3f tempHit{};

	//		Shape* workingTriangle{ nullptr };
	//		
	//		if (triangles[i].IntersectionTest(ray, tempHit, workingTriangle))
	//		{
	//			//tempHit = (localMatrix * Vec4f{ tempHit,1.f }).m_XYZ;

	//			float distanceToHit{ (ray.m_orgin - tempHit).lenght2() };
	//			if (distanceToHit < distance)
	//			{
	//				hit = tempHit;
	//				distance = distanceToHit;
	//				shape = workingTriangle;
	//			}
	//		}
	//	}
	//	if (distance != INFINITY)
	//	{
	//		return true;
	//	}
	//}
	//return false;
}

AABBox* Mesh::getBoundingVolume()
{
	return  nullptr;
}

Color Mesh::getColor(Vec3f& intersectionPoint)
{
	(void)intersectionPoint;
	return { 1.f,1.f,1.f };
}

Vec3f Mesh::getCenter()
{
	return {};
}

Vec3f Mesh::calculateNormal(const Vec3f& hit)
{
	static_cast<void>(hit);
	return { 0.f,0.f,1.f };
	//return activeTriangle->calculateNormal(hit);
}



AABBox Mesh::updateBounds()
{
	Vec3f min{ {INFINITY,INFINITY,-INFINITY} };
	Vec3f max{ {-INFINITY,-INFINITY,INFINITY} };

	for (unsigned i = 0; i < vertices.size(); ++i)
	{
		Vec3f transformedVertice{ (transform.getLocalMatrix() * Vec4f{*vertices[i],1.f}).m_XYZ };

		if (transformedVertice.m_x <= min.m_x)
			min.m_x = transformedVertice.m_x;

		if (transformedVertice.m_x >= max.m_x)
			max.m_x = transformedVertice.m_x;

		if (transformedVertice.m_y <= min.m_y)
			min.m_y = transformedVertice.m_y;

		if (transformedVertice.m_y >= max.m_y)
			max.m_y = transformedVertice.m_y;

		if (transformedVertice.m_z >= min.m_z)
			min.m_z = transformedVertice.m_z;

		if (transformedVertice.m_z <= max.m_z)
			max.m_z = transformedVertice.m_z;
	}

	return AABBox{ min,max };
}

#pragma warning(pop)