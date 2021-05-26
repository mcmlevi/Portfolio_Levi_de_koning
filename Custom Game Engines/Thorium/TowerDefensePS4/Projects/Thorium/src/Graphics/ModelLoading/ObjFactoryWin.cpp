#include <Thpch.h>
#include <Graphics/ModelLoading/ObjFactoryWin.h>
#include <IO/tiny_obj_loader.h>
#include <Resources/Mesh.h>
#include <Components/ModelComponent.h>

#include <Core/MessageLogger.h>
#include <functional>
#include <glm/gtx/hash.hpp>

namespace std
{
    template<>
     struct std::hash<Th::Vertex>
    {
        size_t operator()(Th::Vertex const& vertex) const
        {
            return ((std::hash<glm::vec3>()(vertex.Pos) ^ (std::hash<glm::vec3>()(vertex.Color) << 1)) >> 1);
        }
    };
}

std::string getBaseDir(const std::string& str)
{
	size_t found = str.find_last_of("/\\");
    return str.substr(0, found);
}
namespace Th
{
    ObjFactoryWin::~ObjFactoryWin()
    {
    }

   Th::ModelData* Th::ObjFactoryWin::LoadModel(const std::string& filePath)
    {
	    tinyobj::attrib_t attrib;
	    std::vector<tinyobj::shape_t> shapes;
	    std::vector<tinyobj::material_t> materials;

	    std::string warn;
	    std::string err;

	    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(),getBaseDir(filePath).c_str());

	    if (!warn.empty()) 
	    {
		    LOGWARNING("%s", warn.substr(0, warn.size() - 1).c_str());
	    }

	    if (!err.empty()) 
	    {
		    LOGERROR("%s", err.substr(0,err.size() - 1).c_str());
	    }

	    if (!ret) 
	    {
		    LOGERROR("Something went wrong while trying to load the obj");
	    }

        Th::ModelData* data = new Th::ModelData{};
        std::unordered_map<Vertex, uint32_t> uniqueVertices;

        for (const auto& shape : shapes)
        {
            std::vector<Vertex> vertices;
            std::vector<uint16_t> indicies;
            int materialCounter{};
            int materialIndex{ 0 };
            for (const auto& index : shape.mesh.indices)
            {
                tinyobj::real_t diffuse[3]{1.f,1.f,1.f};
                if (!materials.empty())
                {
                	if(shape.mesh.material_ids[materialIndex] >= 0)
                	{
                        diffuse[0] = materials[shape.mesh.material_ids[materialIndex]].diffuse[0];
                        diffuse[1] = materials[shape.mesh.material_ids[materialIndex]].diffuse[1];
                        diffuse[2] = materials[shape.mesh.material_ids[materialIndex]].diffuse[2];
                	}
                }
                
                Vertex vertex =
                {
                    glm::vec3(attrib.vertices[3 * index.vertex_index + 0],        //Position
                            attrib.vertices[3 * index.vertex_index + 1],
                            attrib.vertices[3 * index.vertex_index + 2]),
                    glm::vec3{diffuse[0],diffuse[1],diffuse[2]}

                };
                
                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                    data->Min = glm::min(data->Min, vertex.Pos);
                    data->Max = glm::max(data->Max, vertex.Pos);
                }

                indicies.push_back(uniqueVertices[vertex]);
                if (materialCounter == 2)
                {
                    ++materialIndex;
                    materialCounter = 0;
                }
            	else
					++materialCounter;
            }
            data->Indexes.push_back(indicies);
            data->Vertices.push_back(vertices);
        }
       
	    return data;
    }
}
