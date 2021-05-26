#pragma once
#include <set>
#include "ModelBuilder.h"
#include "Core/ECSWrapper.h"
#include <CoreRenderAPI/Components/Scene.h>
#include <CoreRenderAPI/Components/Model.h>
#include <CoreRenderAPI/Components/Mesh.h>

#include <tinygltf/tiny_gltf.h>

namespace Frac
{
	/// <summary>
	/// The glTFbuilder is a model builder implementation.
	/// </summary>
	class GLTF_ModelBuilder final : public ModelBuilder
	{
	public:
		GLTF_ModelBuilder(Frac::RenderSystem& renderSystem, PrefabManager& prefabManager, ECSWrapper& entityManager, const std::string& filePath, ELoadType typeOfFileToLoad, bool loadUsingBinary);
		~GLTF_ModelBuilder() = default;

		bool LoadModelFromAPI() override;
		bool ParseModelData() override;

	private:
		enum EExtensionValues
		{
			KHR_LIGHT = 7
		};

		enum class EDrawModes
		{
			POINTS,
			LINES,
			TRIANGLES
		};

		/// <summary>
		/// Mesh used to load 
		/// </summary>
		struct MeshLoadStruct
		{
			EDrawModes DrawMode;
			std::map<TOR::EVertexAttributes, int> Attributes;
			std::vector<TOR::EVertexAttributes> AttributesToLoad;
			int IndiceDataIndex;
			int MaterialDataIndex;
		};

		EntityHandle LoadNode(EntityHandle parent, std::vector<tinygltf::Node>& nodes, 
			std::set<std::string>& extentions, int nodeIndex, bool flipNormals = false);

		EntityHandle AddDefaultCamera(EntityHandle& sceneRoot);
		
		// Component Load functions
		TOR::Transform LoadTransform(tinygltf::Node& nodeToLoad);

		// Shared resources(thus prefabbed in flecs)
		EntityHandle LoadCamera(tinygltf::Node& nodeToLoad,TOR::Transform& transform);
		EntityHandle LoadMesh(tinygltf::Node& nodeToLoad, bool flipNormals, TOR::Transform& transform);
		EntityHandle LoadSkin(tinygltf::Node& nodeToLoad, TOR::Transform& transform);
		EntityHandle LoadLight(tinygltf::Node& nodeToLoad, tinygltf::Value& extentionValue, TOR::Transform& transform);
		EntityHandle LoadTexture(int index, bool isSRGB = false);
		//void LoadExtentions(tinygltf::Node& nodeToLoad, EntityHandle& loadedNode, std::set<std::string>& extentions);

		bool m_LoadUsingBinary;
		tinygltf::Model m_tinyGLTFModel;
	};
}