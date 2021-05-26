#pragma once
#include <string>
#include <vector>

#include <Resources/Mesh.h>
#include <Resources/Vertex.h>

namespace Th
{
	struct ModelData
	{
		std::vector<std::vector<Th::Vertex>> Vertices;
		std::vector<std::vector<std::uint16_t>> Indexes;
		glm::vec3 Min{FLT_MAX,FLT_MAX,FLT_MAX};
		glm::vec3 Max{-FLT_MAX,-FLT_MAX,-FLT_MAX};
	};

	class IModelFactory
	{
	public:
		IModelFactory() = default;
		virtual ~IModelFactory() = default;

		virtual Th::ModelData* LoadModel(const std::string& filePath) = 0;

	};


}

