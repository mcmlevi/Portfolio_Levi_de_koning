#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
namespace Th
{
	struct BoxColliderComponent
	{
		glm::vec3 Min;
		glm::vec3 Max;

		bool intersect(glm::vec3& origin, glm::vec3 dir,glm::mat4 objmat, glm::vec3& hit)
		{
			glm::vec3 newMin = objmat * glm::vec4{ Min,1 };
			glm::vec3 newMax =  objmat * glm::vec4{ Max,1 };
			float divX{ 1.f / dir.x };
			float divY{ 1.f / dir.y };
			float divZ{ 1.f / dir.z };
			int sign[3]{};
			sign[0] = (divX < 0);
			sign[1] = (divY < 0);
			sign[2] = (divZ < 0);
			glm::vec3 points[2]{ {newMin},{newMax} };

			float dMinX{};
			float dMaxX{};
			float dMinY{};
			float dMaxY{};
			float dMinZ{};
			float dMaxZ{};

			dMinX = (points[sign[0]].x - origin.x) * divX;
			dMaxX = (points[1 - sign[0]].x - origin.x) * divX;
			if (dMinX > dMaxX)
				std::swap(dMinX, dMaxX);

			dMinY = (points[sign[1]].y - origin.y) * divY;
			dMaxY = (points[1 - sign[1]].y - origin.y) * divY;

			if (dMinY > dMaxY) std::swap(dMinY, dMaxY);

			if (dMinX > dMaxY || dMinY > dMaxX)
				return false;

			dMinZ = (points[sign[2]].z - origin.z) * divZ;
			dMaxZ = (points[1 - sign[2]].z - origin.z) * divZ;

			if (dMinZ > dMaxZ) std::swap(dMinZ, dMaxZ);

			float dMinXY{ dMinY > dMinX ? dMinY : dMinX };
			float dMaxXY{ dMaxY < dMaxX ? dMaxY : dMaxX };

			if (dMinXY > dMaxZ || dMinZ > dMaxXY)
				return false;

			float dMin{ dMinZ > dMinXY ? dMinZ : dMinXY };
			float dMax{ dMaxZ < dMaxXY ? dMaxZ : dMaxXY };
			if (dMin > dMax) std::swap(dMin, dMax);



			//if (dMin < 0 && dMax >= 0)
			//{
			//	distance = dMin;
			//	return true;
			//}

			if (dMin < 0)
			{
				dMin = dMax;
				if (dMin < 0)
					return false;
			}

			hit = origin + (dir * dMin);
			return true;

			//float t0, t1;
			//glm::vec3 L = glm::vec3{ objmat[3] } - origin;
			//float tca = glm::dot(L, dir);
			//// if (tca < 0) return false;
			//float d2 = glm::dot(L,L) - tca * tca;
			//if (d2 > 1) return false;
			//float thc = sqrt(1 - d2);
			//t0 = tca - thc;
			//t1 = tca + thc;

			//if (t0 > t1) std::swap(t0, t1);

			//if (t0 < 0) {
			//	t0 = t1; // if t0 is negative, let's use t1 instead 
			//	if (t0 < 0) return false; // both t0 and t1 are negative 
			//}

			//hit = origin + (dir * t0);

			//return true;
		}
	};
}
