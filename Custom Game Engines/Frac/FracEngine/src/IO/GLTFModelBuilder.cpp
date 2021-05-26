#include "pch.h"

#include "IO/GLTFModelBuilder.h"
#include "Core/EngineCore.h"
#include "Core/Query.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define STBI_MSC_SECURE_CRT
#include <tinygltf/tiny_gltf.h>

#include "CoreRenderAPI/Components/RenderComponents.h"
#include "CoreRenderAPI/RenderAPI.h"


namespace Frac
{
	GLTF_ModelBuilder::GLTF_ModelBuilder(Frac::RenderSystem& renderSystem, PrefabManager& prefabManager, ECSWrapper& entityManager, const std::string& filePath, ELoadType typeOfFileToLoad
	, bool loadUsingBinary) :
		ModelBuilder(renderSystem, prefabManager, entityManager, filePath, typeOfFileToLoad),
		m_LoadUsingBinary(loadUsingBinary)
	{ }

	bool GLTF_ModelBuilder::LoadModelFromAPI()
	{
		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;
		bool ret = m_LoadUsingBinary 
			? loader.LoadBinaryFromFile(&m_tinyGLTFModel, &err, &warn, std::string(m_ModelDirectory + m_ModelName + ".glb"))
			: loader.LoadASCIIFromFile(&m_tinyGLTFModel, &err, &warn, std::string(m_ModelDirectory + m_ModelName + ".gltf"));

		if (!warn.empty()) {
			LOGWARNING("%s\n", warn.c_str());
		}

		if (!err.empty()) {
			LOGERROR("%s\n", err.c_str());
		}

		if (!ret) {
			LOGINFO("Failed to parse glTF\n");
			return false;
		}

		return true;
	}

	bool GLTF_ModelBuilder::ParseModelData()
	{
		std::vector<tinygltf::Node, std::allocator<tinygltf::Node>>& nodes = m_tinyGLTFModel.nodes;
		for (int i = 0; i < m_tinyGLTFModel.scenes.size(); ++i)
		{
			tinygltf::Scene& sceneToLoad = m_tinyGLTFModel.scenes[i];
			std::string sceneName = m_tinyGLTFModel.scenes[i].name;
			EntityHandle scene = m_entityManager.makeEntity("Scene of : " + m_ModelName);

			std::set<std::string> extensionBuffer;
			for (auto& extension : m_tinyGLTFModel.extensionsUsed)
			{
				extensionBuffer.insert(extension);
			}

			for (auto& extension : m_tinyGLTFModel.extensionsRequired)
			{
				extensionBuffer.insert(extension);
			}

			for (int nodeIndex = 0; nodeIndex < sceneToLoad.nodes.size(); ++nodeIndex)
			{
				m_entityManager.addChild(LoadNode(scene, nodes, extensionBuffer, sceneToLoad.nodes[nodeIndex]),scene);
			}

			switch (m_builderType)
			{
			case ELoadType::MODEL:
			{
				m_finalModel = scene;
				break;
			}
			}
		}
		return true;
	}

	EntityHandle GLTF_ModelBuilder::LoadNode(EntityHandle parent, std::vector<tinygltf::Node>& nodes, std::set<std::string>& extensions, int nodeIndex, bool flipNormals)
	{
		tinygltf::Node& nodeToLoad = nodes[nodeIndex];
		EntityHandle loadedEntity;
		TOR::Transform transformOfNode = LoadTransform(nodeToLoad);

		if (nodeToLoad.camera != -1)
		{
			//loadedEntity = LoadCamera(nodeToLoad,transformOfNode);
		}
		else if (nodeToLoad.mesh != -1)
		{
			loadedEntity = LoadMesh(nodeToLoad, flipNormals, transformOfNode); // this actually loads models :P
		}
		else if (nodeToLoad.skin != -1)
		{
			loadedEntity = LoadSkin(nodeToLoad, transformOfNode);
		}
		else
		{
			if(nodeToLoad.extensions.size() > 0)
			{
				for (auto& extension : extensions)
				{
					if (nodeToLoad.extensions.find(extension) == nodeToLoad.extensions.end())
					{
						continue;
					}

					tinygltf::Value extensionValue = nodeToLoad.extensions[extension];
					if (extensionValue.Type() != 0)
					{
						switch (extensionValue.Type())
						{
						case EExtensionValues::KHR_LIGHT:
						{
							loadedEntity = LoadLight(nodeToLoad, extensionValue, transformOfNode);
							break;
						}
						}
					}
				}
			}
			else
			{
				loadedEntity = m_entityManager.makeEntity(nodeToLoad.name);
				m_entityManager.set(loadedEntity, transformOfNode);
			}
		}
		
		//m_entityManager.addChild(loadedEntity, parent);
		for (int childToLoadIndex = 0; childToLoadIndex < nodeToLoad.children.size(); ++childToLoadIndex)
		{
			EntityHandle child = LoadNode(loadedEntity, nodes, extensions, nodeToLoad.children[childToLoadIndex]);
			m_entityManager.addChild(child, loadedEntity);
		}

		return loadedEntity;
	}

	EntityHandle GLTF_ModelBuilder::AddDefaultCamera(EntityHandle& sceneRoot)
	{
		std::string prefabName = std::string("DEFAULT_CAMERA");
		if (m_prefabManager.HasPrefab(prefabName))
		{
			EntityHandle defaultCamHandle = m_prefabManager.GetPrefabInstance(prefabName);
			m_entityManager.addChild(defaultCamHandle, sceneRoot);

			LOGWARNING("Loaded the default camera with a duplicated name!");
			return defaultCamHandle;
		}

		//#TODO: Camera width and height is not being loaded from somewhere. Agreed on having it 1820 768 for now.
		TOR::Transform defaultCameraTransform;
		defaultCameraTransform.Position = glm::vec3(-2, 0, 5);
		defaultCameraTransform.ModelTransformData = glm::lookAtRH(
			defaultCameraTransform.Position,
			glm::vec3(0.0f),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		//#TODO: The camera component creation is a duplicate from the GLTF_ModelBuilder::LoadCamera method. Add a factory method somewhere along the line.
		float fovInDeg = 70.0f;
		float zNear = 0.01f;
		float zFar = 1000.0f;
		float screenWidth = 1280.0f;
		float screenHeight = 768.0f;
		float aspectRatio = screenHeight / screenHeight;

		TOR::Camera defaultCameraProps =
		{
			glm::perspectiveRH(glm::radians(fovInDeg), aspectRatio, zNear, zFar),

			fovInDeg,
			zNear,
			zFar,
			aspectRatio,

			static_cast<int>(screenWidth),
			static_cast<int>(screenHeight),

			TOR::ECameraType::Perspective,
			false
		};

		//#TODO: Camera from glTF gives you yfov, current camera expects XFoV.
		EntityHandle defaultCamera = m_entityManager.makeEntity(prefabName, defaultCameraTransform, defaultCameraProps);
		m_prefabManager.CachePrefab(prefabName, defaultCamera);
		m_entityManager.addChild(defaultCamera, sceneRoot);

		return m_prefabManager.GetPrefabInstance(prefabName);
	}
	
	TOR::Transform GLTF_ModelBuilder::LoadTransform(tinygltf::Node& nodeToLoad)
	{
		TOR::Transform transform;

		glm::vec3 positionOfNode(0);
		glm::quat rotationOfNode = glm::quat_identity<float, glm::defaultp>();
		glm::vec3 scaleOfNode{ 1,1,1 };

		glm::mat4x4 localMatrixOfNode;

		if (nodeToLoad.matrix.size() != 0)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					localMatrixOfNode[i][j] = static_cast<float>(nodeToLoad.matrix[i + (j * 4)]);
				}
			}

			//#TODO: Unit test the decompose, this was implemented in the PoC, but did not work as intended.
			glm::vec3 skew;
			glm::vec4 perspective;
			glm::decompose(localMatrixOfNode, transform.Scale, transform.Orientation, transform.Position, skew, perspective);
			transform.ModelTransformData = glm::identity<glm::mat4x4>();
		}
		else
		{
			if (nodeToLoad.translation.size() != 0)
			{
				transform.Position = 
				{ 
					static_cast<float>(nodeToLoad.translation[0]),
					static_cast<float>(nodeToLoad.translation[1]),
					static_cast<float>(nodeToLoad.translation[2])
				};
			}
			else
			{
				transform.Position =
				{
					0,0,0
				};
			}
			if (nodeToLoad.rotation.size() != 0)
			{
				transform.Orientation = 
				{
					static_cast<float>(nodeToLoad.rotation[3]), 
					static_cast<float>(nodeToLoad.rotation[0]),
					static_cast<float>(nodeToLoad.rotation[1]),
					static_cast<float>(nodeToLoad.rotation[2])
				};
			}
			else
			{
				transform.Orientation =
				{
					1,0,0,0
				};
			}
			if (nodeToLoad.scale.size() != 0)
			{
				transform.Scale =
				{ 
					static_cast<float>(nodeToLoad.scale[0]),
					static_cast<float>(nodeToLoad.scale[1]),
					static_cast<float>(nodeToLoad.scale[2])
				};
			}
			else
			{
				transform.Scale =
				{
					1,1,1
				};
			}
		}
		return transform;
	}

	EntityHandle GLTF_ModelBuilder::LoadCamera(tinygltf::Node& nodeToLoad, TOR::Transform& transform)
	{
		std::string prefabName = std::string(m_ModelName + "_CAMERA_" + std::to_string(nodeToLoad.camera));
		if (m_prefabManager.HasPrefab(prefabName))
		{
			EntityHandle newInstanceOfCamera = m_prefabManager.GetPrefabInstance(prefabName);
			LOGWARNING("Loaded in a camera with a duplicated name!");
			return newInstanceOfCamera;
		}

		tinygltf::Camera& loadedCamera = m_tinyGLTFModel.cameras[nodeToLoad.camera];

		TOR::Camera cameraProps;
		if(loadedCamera.type == "PERSPECTIVE")
		{
			float screenWidth = 1280.0f;
			float screenHeight = 768.0f;

			cameraProps.FovInDeg = 35.0f;
			cameraProps.ZNear = loadedCamera.perspective.znear;
			cameraProps.ZFar = loadedCamera.perspective.zfar;
			cameraProps.AspectRatio = screenWidth / screenHeight;

			cameraProps.ScreenWidth = static_cast<int>(screenWidth);
			cameraProps.ScreenHeight = static_cast<int>(screenHeight);

			cameraProps.CameraType = TOR::ECameraType::Perspective;
			cameraProps.ProjectionMat = glm::perspectiveRH(glm::radians(cameraProps.FovInDeg), cameraProps.AspectRatio, cameraProps.ZNear, cameraProps.ZFar);
		}
		else //Orthographic
		{
			//#TODO: Implement orthographic camera input.
		}

		EntityHandle newLoadedCamera = m_entityManager.makeEntity(prefabName);
		m_entityManager.set(newLoadedCamera, cameraProps);
		m_entityManager.set(newLoadedCamera, transform);
		m_prefabManager.CachePrefab(prefabName, newLoadedCamera);

		return m_prefabManager.GetPrefabInstance(prefabName);
	}

	EntityHandle GLTF_ModelBuilder::LoadMesh(tinygltf::Node& nodeToLoad, bool flipNormals, TOR::Transform& transform)
	{
		/// <summary>
		/// We need to create an entity prefab that contains a flat hierarchy under it with all meshes that belong to it.
		/// this entity prefab would be the model, in glTF that is called a mesh. what a mesh is for us is in glTF a primitive.
		/// A model prefab entity should have its own Transform component.
		/// A mesh entity should have a material attached to it, preferable an instance of a material prefab.
		/// </summary>

		std::string prefabModelName = std::string(m_ModelName + "_COMPLETE_MODEL_" + std::to_string(nodeToLoad.mesh));
		EntityHandle ModelEntity;
		SubResourceContainer ModelSubresourcesContainer;
		TOR::RenderAPI& renderAPI = m_renderSystem.GetRenderAPI();
		int meshArrayIndex = nodeToLoad.mesh;
		tinygltf::Mesh& tinyGLTFMesh = m_tinyGLTFModel.meshes[meshArrayIndex];

		if(!m_prefabManager.HasPrefab(prefabModelName))
		{
			ModelEntity = m_entityManager.makeEntity(prefabModelName);
			for (int i = 0; i < tinyGLTFMesh.primitives.size(); i++)
			{
				std::string prefabMeshName = std::string(m_ModelName + "_MESH_" + std::to_string(nodeToLoad.mesh) + "_" + std::to_string(i) + "_" + "MATERIAL_" + std::to_string(tinyGLTFMesh.primitives[i].material));
				EntityHandle loadedMesh;
				if (m_prefabManager.HasPrefab(prefabMeshName)) 
				{
					loadedMesh = m_prefabManager.GetPrefab(prefabMeshName);
				}
				else 
				{
					MeshLoadStruct newPrimitivetoLoad;
					switch (tinyGLTFMesh.primitives[i].mode)
					{
					case 0:
						newPrimitivetoLoad.DrawMode = EDrawModes::POINTS;
						spdlog::error("This draw mode is not implemented");
						break;

					case 1:
						newPrimitivetoLoad.DrawMode = EDrawModes::LINES;
						spdlog::error("This draw mode is not implemented");
						break;

					case 2:
						spdlog::error("This draw mode is not implemented");
						break;

					case 3:
						spdlog::error("This draw mode is not implemented");
						break;

					case 4:
						newPrimitivetoLoad.DrawMode = EDrawModes::TRIANGLES;
						break;

					case 5:
						spdlog::error("This draw mode is not implemented");
						break;

					case 6:
						spdlog::error("This draw mode is not implemented");
						break;

					default:
						newPrimitivetoLoad.DrawMode = GLTF_ModelBuilder::EDrawModes::TRIANGLES;
						break;
					}

					newPrimitivetoLoad.IndiceDataIndex = tinyGLTFMesh.primitives[i].indices;
					newPrimitivetoLoad.MaterialDataIndex = tinyGLTFMesh.primitives[i].material;

					if (tinyGLTFMesh.primitives[i].attributes.find("POSITION") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::POSITION] = tinyGLTFMesh.primitives[i].attributes.at("POSITION");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::POSITION);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("NORMAL") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::NORMAL] = tinyGLTFMesh.primitives[i].attributes.at("NORMAL");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::NORMAL);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("TEXCOORD_0") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::TEXCOORD_0] = tinyGLTFMesh.primitives[i].attributes.at("TEXCOORD_0");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::TEXCOORD_0);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("TEXCOORD_1") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::TEXCOORD_1] = tinyGLTFMesh.primitives[i].attributes.at("TEXCOORD_1");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::TEXCOORD_1);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("TANGENT") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::TANGENT] = tinyGLTFMesh.primitives[i].attributes.at("TANGENT");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::TANGENT);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("COLOR_0") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::COLOR_0] = tinyGLTFMesh.primitives[i].attributes.at("COLOR_0");
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::COLOR_0);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("JOINTS_0") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::JOINTS_0] = tinyGLTFMesh.primitives[i].attributes.at("JOINTS_0");
						spdlog::critical("not implemented"); assert(false);
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::JOINTS_0);
					}

					if (tinyGLTFMesh.primitives[i].attributes.find("WEIGHTS_0") != tinyGLTFMesh.primitives[i].attributes.end())
					{
						newPrimitivetoLoad.Attributes[TOR::EVertexAttributes::WEIGHTS_0] = tinyGLTFMesh.primitives[i].attributes.at("WEIGHTS_0");
						spdlog::critical("not implemented"); assert(false);
						newPrimitivetoLoad.AttributesToLoad.push_back(TOR::EVertexAttributes::WEIGHTS_0);
					}
			
					//Create the mesh.
					TOR::Mesh meshComponent;
					renderAPI.CreateMesh(meshComponent);

					// Load attributes
					auto& accessors = m_tinyGLTFModel.accessors;
					for (TOR::EVertexAttributes& attr : newPrimitivetoLoad.AttributesToLoad)
					{
						auto& accessorToLoadDataWith = accessors[newPrimitivetoLoad.Attributes[attr]];
						auto& bufferView = m_tinyGLTFModel.bufferViews[accessorToLoadDataWith.bufferView];
						auto& bufferToLoadDataFrom = m_tinyGLTFModel.buffers[bufferView.buffer];

						uint32_t amountOfBytesToLoad = accessorToLoadDataWith.count;

						int numberOfComponents;
						switch (accessorToLoadDataWith.type)
						{
						case TINYGLTF_TYPE_SCALAR:
							numberOfComponents = 1;
							break;
						case TINYGLTF_TYPE_VEC2:
							numberOfComponents = 2;
							amountOfBytesToLoad *= 8;
							break;
						case TINYGLTF_TYPE_VEC3:
							numberOfComponents = 3;
							amountOfBytesToLoad *= (numberOfComponents * 4);
							break;
						case TINYGLTF_TYPE_VEC4:
							numberOfComponents = 4;
							amountOfBytesToLoad *= (numberOfComponents * 4);
							break;
						case TINYGLTF_TYPE_MAT2:
							numberOfComponents = 4;
							amountOfBytesToLoad *= (numberOfComponents * 4);
							break;
						case TINYGLTF_TYPE_MAT3:
							numberOfComponents = 9;
							amountOfBytesToLoad *= (numberOfComponents * 4);
							break;
						case TINYGLTF_TYPE_MAT4:
							numberOfComponents = 16;
							amountOfBytesToLoad *= (numberOfComponents * 4);
							break;
						}

						if (attr == TOR::EVertexAttributes::POSITION)
						{
							meshComponent.VertexCount = bufferView.byteLength / sizeof(glm::vec3);
						}

						renderAPI.BufferAttributeData(meshComponent.Id, attr,
							amountOfBytesToLoad, bufferView.byteStride, 
							numberOfComponents, 
							&bufferToLoadDataFrom.data.at(bufferView.byteOffset + accessorToLoadDataWith.byteOffset), 
							static_cast<TOR::EValueTypes>(accessorToLoadDataWith.componentType), //Need to make sure that this is the correct number! might only work for OpenGL
							TOR::EDrawTypes::STATIC_DRAW);
					}

					// load indices
					auto& accessorToLoadDataWith = accessors[newPrimitivetoLoad.IndiceDataIndex];
					auto& bufferView = m_tinyGLTFModel.bufferViews[accessorToLoadDataWith.bufferView];
					auto& bufferToLoadDataFrom = m_tinyGLTFModel.buffers[bufferView.buffer];
					int elementSize = 0;
					switch (accessorToLoadDataWith.componentType)
					{
					case TINYGLTF_COMPONENT_TYPE_BYTE:
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						meshComponent.ElementDataType = TOR::ETorDataType::BYTE;
						elementSize = 1;
						break;
					case TINYGLTF_COMPONENT_TYPE_SHORT:
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						meshComponent.ElementDataType = TOR::ETorDataType::USHORT;
						elementSize = 2;
						break;
					case TINYGLTF_COMPONENT_TYPE_FLOAT:
					case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						meshComponent.ElementDataType = TOR::ETorDataType::FLOAT;
						elementSize = 4;
						break;
					default:
						assert(false);
					}

					meshComponent.IndiceCount = accessorToLoadDataWith.count;
					renderAPI.BufferElementData(meshComponent.Id, accessorToLoadDataWith.count * elementSize,
						&bufferToLoadDataFrom.data.at(bufferView.byteOffset + accessorToLoadDataWith.byteOffset), 
						TOR::EDrawTypes::DYNAMIC_DRAW);

					renderAPI.UnbindMesh(meshComponent.Id);


					EntityHandle newLoadedMesh = m_entityManager.makePrefab(prefabMeshName);
					m_entityManager.set(newLoadedMesh,meshComponent);
					m_prefabManager.CachePrefab(prefabMeshName, newLoadedMesh);

					loadedMesh = m_prefabManager.GetPrefab(prefabMeshName);
				}
		
		
				//Load material; cache if new otherwise retrieve.
				int MaterialNr = tinyGLTFMesh.primitives[i].material;
				bool isPBRMaterial = false;
				EntityHandle MeshMaterial;

				std::string materialName = m_ModelName + "_MATERIAL_" + std::to_string(MaterialNr);
				if (!m_prefabManager.HasPrefab(materialName))
				{

					SubResourceContainer materialSubresourcesContainer;
					std::string textureName;
					EntityHandle newLoadedMaterial = m_entityManager.makeEntity(materialName);
					TOR::Material materialComponent;
					tinygltf::Material MaterialToLoad = m_tinyGLTFModel.materials[MaterialNr];

					//load Material values
					if(MaterialToLoad.alphaMode == "OPAQUE")
					{
						materialComponent.AlphaMode = TOR::EAlphaMode::OPAQUE_MODE;
					}
					else if(MaterialToLoad.alphaMode == "BLEND")
					{
						materialComponent.AlphaMode = TOR::EAlphaMode::BLEND_MODE;
					}
					else if(MaterialToLoad.alphaMode == "MASK")
					{
						//Defaulted to 0.5, only needs to be be set when the mask is applied.
						materialComponent.AlphaMode = TOR::EAlphaMode::MASK_MODE;
						materialComponent.AlphaCutoff = MaterialToLoad.alphaCutoff;
					}
					else
					{
						LOGWARNING("'%s' is an invalid alpha mode! OPAQUE will be used.");
						materialComponent.AlphaMode = TOR::EAlphaMode::OPAQUE_MODE;
					}

					//Double sided planes.
					materialComponent.DoubleSided = MaterialToLoad.doubleSided;
					
					//Albedo
					if (MaterialToLoad.pbrMetallicRoughness.baseColorTexture.index != -1)
					{
						EntityHandle newLoadedTexture = LoadTexture(MaterialToLoad.pbrMetallicRoughness.baseColorTexture.index, true);
						
						textureName = m_ModelName + "_REUSED_TEXTURE_" + m_tinyGLTFModel.textures[MaterialToLoad.pbrMetallicRoughness.baseColorTexture.index].name + "_" + std::to_string(MaterialToLoad.pbrMetallicRoughness.baseColorTexture.index);
						materialSubresourcesContainer.prefabResources.push_back(std::make_pair(textureName, SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
						materialComponent.TexCoords[static_cast<int>(TOR::ETextureType::ALBEDO)] = MaterialToLoad.pbrMetallicRoughness.baseColorTexture.texCoord;
						m_entityManager.set<TOR::AlbedoTexture>(newLoadedMaterial,{*m_entityManager.get<TOR::Texture>(newLoadedTexture)});
					}
					else
					{
						materialComponent.AlbedoFactor = glm::vec4(
							static_cast<float>(MaterialToLoad.pbrMetallicRoughness.baseColorFactor[0]),
							static_cast<float>(MaterialToLoad.pbrMetallicRoughness.baseColorFactor[1]),
							static_cast<float>(MaterialToLoad.pbrMetallicRoughness.baseColorFactor[2]),
							static_cast<float>(MaterialToLoad.pbrMetallicRoughness.baseColorFactor[3])
						);
					}

					//Metallic Roughness
					if (MaterialToLoad.pbrMetallicRoughness.metallicRoughnessTexture.index != -1)
					{
						EntityHandle newLoadedTexture = LoadTexture(MaterialToLoad.pbrMetallicRoughness.metallicRoughnessTexture.index);

						textureName = m_ModelName + "_REUSED_TEXTURE_" + m_tinyGLTFModel.textures[MaterialToLoad.pbrMetallicRoughness.metallicRoughnessTexture.index].name + "_" + std::to_string(MaterialToLoad.pbrMetallicRoughness.metallicRoughnessTexture.index);
						materialSubresourcesContainer.prefabResources.push_back(std::make_pair(textureName, SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
						materialComponent.TexCoords[static_cast<int>(TOR::ETextureType::METALROUGHNESS)] = MaterialToLoad.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
						m_entityManager.set<TOR::MetalRougnessAOTexture>(newLoadedMaterial, { *m_entityManager.get<TOR::Texture>(newLoadedTexture) });
					}
					else
					{
						//LOGINFO("No PBR texture found, using the predefined Metallic and roughness factors.");
						materialComponent.MetallicFactor = MaterialToLoad.pbrMetallicRoughness.metallicFactor;
						materialComponent.RoughnessFactor = MaterialToLoad.pbrMetallicRoughness.roughnessFactor;
					}

					//Normal
					if (MaterialToLoad.normalTexture.index != -1) 
					{
						EntityHandle newLoadedTexture = LoadTexture(MaterialToLoad.normalTexture.index);

						textureName = m_ModelName + "_REUSED_TEXTURE_" + m_tinyGLTFModel.textures[MaterialToLoad.normalTexture.index].name + "_" + std::to_string(MaterialToLoad.normalTexture.index);
						materialSubresourcesContainer.prefabResources.push_back(std::make_pair(textureName, SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
						materialComponent.TexCoords[static_cast<int>(TOR::ETextureType::NORMAL)] = MaterialToLoad.normalTexture.texCoord;

						//scaledNormal = normalize((<sampled normal texture value>* 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))
						materialComponent.NormalScale = static_cast<float>(MaterialToLoad.normalTexture.scale);
						m_entityManager.set<TOR::NormalTexture>(newLoadedMaterial, { *m_entityManager.get<TOR::Texture>(newLoadedTexture) });

					}
					else
					{
						//LOGWARNING("Normal mapping is not provided by material: '%s' and will therefor not be used.");
					}

					//Occlusion
					if (MaterialToLoad.occlusionTexture.index != -1)
					{
						
						materialComponent.HasOcclusionMap = true;
					}
					else
					{
						//LOGWARNING("Occlusion map is not provided by material: '%s' and will therefor not be used.");
					}

					//Emission
					if (MaterialToLoad.emissiveTexture.index != -1)
					{
						TOR::Texture emissionMap;
						EntityHandle newLoadedTexture = LoadTexture(MaterialToLoad.emissiveTexture.index);
						
						textureName = m_ModelName + "_REUSED_TEXTURE_" + m_tinyGLTFModel.textures[MaterialToLoad.emissiveTexture.index].name + "_" + std::to_string(MaterialToLoad.emissiveTexture.index);
						materialSubresourcesContainer.prefabResources.push_back(std::make_pair(textureName,SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
						materialComponent.EmissionFactor = glm::vec3(MaterialToLoad.emissiveFactor[0], MaterialToLoad.emissiveFactor[1], MaterialToLoad.emissiveFactor[2]);
						materialComponent.TexCoords[static_cast<int>(TOR::ETextureType::EMISSION)] = MaterialToLoad.emissiveTexture.texCoord;
						m_entityManager.set<TOR::EmissionTexture>(newLoadedMaterial, { *m_entityManager.get<TOR::Texture>(newLoadedTexture) });
					}
					else
					{
						//LOGWARNING("Emission map is not provided by material: '%s' and will therefor not be used.");
					}
					materialComponent.FlipNormals = flipNormals;
					m_entityManager.set(newLoadedMaterial,materialComponent);
					m_entityManager.set(newLoadedMaterial,materialSubresourcesContainer);
					m_prefabManager.CachePrefab(materialName,newLoadedMaterial);
				
				}
			
				
				
				SubResourceContainer meshSubResources;
				meshSubResources.prefabResources.push_back(std::make_pair(materialName,SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
				m_entityManager.set(loadedMesh,meshSubResources);

				MeshMaterial = m_prefabManager.GetPrefab(materialName);
				m_entityManager.AddInstancedPrefabToEntity(MeshMaterial, loadedMesh);
				isPBRMaterial = m_entityManager.has<TOR::MetalRougnessAOTexture>(MeshMaterial);
			
				if(isPBRMaterial == true)
				{
					m_renderSystem.GetRenderAPI().AssignShaderToMesh(m_entityManager.get<TOR::Mesh>(loadedMesh)->Id,TOR::EShaderType::DiffusePBR);
				}
				else 
				{
					m_renderSystem.GetRenderAPI().AssignShaderToMesh(m_entityManager.get<TOR::Mesh>(loadedMesh)->Id, TOR::EShaderType::DiffusePBR);
				}

				//Add loaded mesh to model class.
				//m_entityManager.addChild(loadedMesh, ModelEntity);
				ModelSubresourcesContainer.prefabResources.push_back(std::make_pair(prefabMeshName,SubResourceContainer::TypeOfSubresource::CHILD));
			} 
			m_entityManager.set(ModelEntity,ModelSubresourcesContainer);
			m_entityManager.set(ModelEntity, transform);
			m_prefabManager.CachePrefab(prefabModelName,ModelEntity);
		}
		ModelEntity = m_prefabManager.GetPrefabInstance(prefabModelName);
		std::cout << m_entityManager.getName(ModelEntity) <<"\n";
		return ModelEntity;
	}

	EntityHandle GLTF_ModelBuilder::LoadLight(tinygltf::Node& nodeToLoad, tinygltf::Value& extensionValue, TOR::Transform& transform)
	{
		tinygltf::Light tinyGltfLight = m_tinyGLTFModel.lights[extensionValue.Get("light").GetNumberAsInt()];
		EntityHandle newLight = m_entityManager.makeEntity(m_ModelName + "_LIGHT_" + std::to_string(extensionValue.Get("light").GetNumberAsInt()));

		//Babylon exporter fix - when the color is white the exporter ignores it.
		glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
		if (tinyGltfLight.color.capacity() > 0)
		{
			color = glm::vec3(
				static_cast<float>(tinyGltfLight.color[0]),
				static_cast<float>(tinyGltfLight.color[1]),
				static_cast<float>(tinyGltfLight.color[2]));
		}

		if (tinyGltfLight.type == "point")
		{
			TOR::PointLight pointLight;
			pointLight.Color = color;
			pointLight.Intensity = static_cast<float>(tinyGltfLight.intensity);
			pointLight.Radius = tinyGltfLight.range > 0.0f ? static_cast<float>(tinyGltfLight.range) : 1.0f;
		
			m_entityManager.set(newLight, pointLight);
		}
		else if (tinyGltfLight.type == "directional")
		{
			TOR::DirectionalLight directionalLight;
			directionalLight.Color = color;
			directionalLight.Intensity = static_cast<float>(tinyGltfLight.intensity);
			
			m_entityManager.set(newLight, directionalLight);
		}
		else if (tinyGltfLight.type == "spot")
		{
			TOR::SpotLight spotLight;
			spotLight.Color = color;
			spotLight.Intensity = static_cast<float>(tinyGltfLight.intensity);
			spotLight.InnerCone = static_cast<float>(tinyGltfLight.spot.innerConeAngle);
			spotLight.OuterCone = static_cast<float>(tinyGltfLight.spot.outerConeAngle);

			m_entityManager.set(newLight, spotLight);
		}
		else
		{
			LOGWARNING("WARNING: Unsupported light type encountered\n");
		}
		m_entityManager.set(newLight, transform);
		return newLight;
	}

	EntityHandle GLTF_ModelBuilder::LoadSkin(tinygltf::Node& nodeToLoad, TOR::Transform& transform)
	{
		int skinArrayIndex = nodeToLoad.skin;
		tinygltf::Skin& skinOfNodeThatIsBeingLoaded = m_tinyGLTFModel.skins[skinArrayIndex];
		LOGWARNING("Ignoring glTF skin, implementation missing.");
		return EntityHandle();
	}

	EntityHandle GLTF_ModelBuilder::LoadTexture(int index, bool isSRGB)
	{
		TOR::RenderAPI& renderAPI = m_renderSystem.GetRenderAPI();
		tinygltf::Texture tinyGltfTexture = m_tinyGLTFModel.textures[index];
		tinygltf::Image tinyGltfImage = m_tinyGLTFModel.images[tinyGltfTexture.source];
		//#TODO: Check if this tinyGlTF texture has already been loaded in, if so use the shared data.
		std::string imageName = m_ModelName + "_" + std::to_string(index) +"_REUSED_IMAGE_" + tinyGltfImage.uri;
		std::string samplerName;
		if (tinyGltfTexture.sampler != -1)
		{
			samplerName = m_ModelName + "REUSED_SAMPLER_" + std::to_string(tinyGltfTexture.sampler);
		}
		else
		{
			samplerName = "REUSED_SAMPLER_DEFAULT";
		}

		std::string textureName = m_ModelName + "_REUSED_TEXTURE_"+ tinyGltfTexture.name + "_" + std::to_string(index);

		EntityHandle TextureInstanceToReturn;

		if(!m_prefabManager.HasPrefab(textureName))
		{
			EntityHandle imageEntity; // ENTITY THAT HAS IMAGE COMPONENT FOR TEXTURE ENTITY
			if (!m_prefabManager.HasPrefab(imageName))
			{
				EntityHandle newEntity = m_entityManager.makeEntity(imageName);
				TOR::Image imageOfTexture =
				{
					tinyGltfImage.uri.c_str(),
					tinyGltfImage.width,
					tinyGltfImage.height,
					tinyGltfImage.component,
					tinyGltfImage.bits
				};

				m_entityManager.set(newEntity,imageOfTexture);
				m_prefabManager.CachePrefab(imageName,newEntity);
			}
			imageEntity = m_prefabManager.GetPrefab(imageName);

			EntityHandle samplerEntity; // ENTITY THAT HAS SAMPLER COMPONENT FOR TEXTURE ENTITY
			if (m_prefabManager.HasPrefab(samplerName))
			{
				samplerEntity = m_prefabManager.GetPrefab(samplerName);
			}
			else
			{
				TOR::Sampler sampler;
				EntityHandle newSampler = m_entityManager.makeEntity(samplerName);
				if (tinyGltfTexture.sampler != -1)
				{
					tinygltf::Sampler tinyGltfSampler = m_tinyGLTFModel.samplers[tinyGltfTexture.sampler];
					sampler =
					{
						tinyGltfSampler.minFilter > -1 ? TOR::Sampler::ConvertglTFFilterMethodToTORFilterMethod(tinyGltfSampler.minFilter) : TOR::ESampleFilterMethod::LINEAR_MIPMAP_LINEAR,
						tinyGltfSampler.magFilter > -1 ? TOR::Sampler::ConvertglTFFilterMethodToTORFilterMethod(tinyGltfSampler.magFilter) : TOR::ESampleFilterMethod::LINEAR,
						tinyGltfSampler.wrapS > -1 ? TOR::Sampler::ConvertglTFWrapMethodToTORWrapMethod(tinyGltfSampler.wrapS) : TOR::ETextureWrapMethod::REPEAT,
						tinyGltfSampler.wrapT > -1 ? TOR::Sampler::ConvertglTFWrapMethodToTORWrapMethod(tinyGltfSampler.wrapT) : TOR::ETextureWrapMethod::REPEAT,
						tinyGltfSampler.wrapR > -1 ? TOR::Sampler::ConvertglTFWrapMethodToTORWrapMethod(tinyGltfSampler.wrapR) : TOR::ETextureWrapMethod::REPEAT,
					};
				}
				else
				{
					sampler =
					{
						TOR::ESampleFilterMethod::LINEAR_MIPMAP_LINEAR,
						TOR::ESampleFilterMethod::LINEAR,
						TOR::ETextureWrapMethod::REPEAT,
						TOR::ETextureWrapMethod::REPEAT,
						TOR::ETextureWrapMethod::REPEAT
					};
				}
				m_entityManager.set(newSampler,sampler);
				m_prefabManager.CachePrefab(samplerName,newSampler);
				samplerEntity = m_prefabManager.GetPrefab(samplerName);
			}

			TOR::Texture texture;
			TOR::EImageChannelType format = isSRGB ? TOR::EImageChannelType::SRGB8_ALPHA8 : TOR::EImageChannelType::RGBA;
			renderAPI.CreateTexture(texture);
			renderAPI.LoadTexture(texture.Id, *m_entityManager.get<TOR::Image>(imageEntity), tinyGltfImage.image.data(), tinyGltfImage.image.size(), *m_entityManager.get<TOR::Sampler>(samplerEntity), static_cast<int>(format)); // should we cast format? you will be casting it in render api aswell this way @ Tom.
		
			EntityHandle newTexture = m_entityManager.makeEntity(textureName);
			m_entityManager.set(newTexture,texture);

			SubResourceContainer newContainer;
			newContainer.prefabResources.push_back(std::make_pair(imageName,SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
			newContainer.prefabResources.push_back(std::make_pair(samplerName, SubResourceContainer::TypeOfSubresource::ATTACHEMENT));
			m_entityManager.set(newTexture, newContainer);

			//m_entityManager.AddInstancedPrefabToEntity(imageEntity,newTexture);
			//m_entityManager.AddInstancedPrefabToEntity(samplerEntity,newTexture);

			
	
			m_prefabManager.CachePrefab(textureName,newTexture);
			
		}
		TextureInstanceToReturn = m_prefabManager.GetPrefab(textureName);
		return TextureInstanceToReturn;
	}
}