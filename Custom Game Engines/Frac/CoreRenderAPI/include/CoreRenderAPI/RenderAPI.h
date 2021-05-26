#pragma once

#include "CoreRenderAPI/LowLevelAPI.h"
#include "CoreRenderAPI/Components/Mesh.h"
#include "CoreRenderAPI/Components/Material.h"
#include "CoreRenderAPI/Components/Light.h"
#include "CoreRenderAPI/Components/Camera.h"

namespace TOR
{
	/// <summary>
	/// The RenderAPi is used to load and manage the low-end graphics API, which also handles the graphics API specific
	/// methods.
	/// </summary>
	class RenderAPI
	{
	public:
		static RenderAPI* CreateRenderAPI(const char* dllName);

		 virtual void Initialize(const std::string& assetDirectory, unsigned viewportWidth, unsigned viewportHeight);
		 virtual void Shutdown();

		//Mesh methods.
		 virtual void CreateMesh(Mesh& meshStructToFill);
		 virtual void DestroyMesh(unsigned meshId);
		 virtual void BindMeshForRecording(unsigned meshId);
		 virtual void UnbindMesh(unsigned meshId);
		 virtual void BufferAttributeData(unsigned meshId, EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, EValueTypes DataType = EValueTypes::FLOAT, EDrawTypes DrawMode = EDrawTypes::STATIC_DRAW);
		 virtual void BufferElementData(unsigned meshId, uint32_t byteLength, void* bufferStart, EDrawTypes DrawMode);

		//Texture methods.
		 virtual void CreateTexture(Texture& texture);
		 virtual void LoadTexture(unsigned textureId, const Image& image, unsigned char* source, int sourceCountInElements, const Sampler& sampler, int format);
		 virtual void DestroyTexture(unsigned textureId);
		 virtual void BindTexture(unsigned textureId, unsigned int slot);
		 virtual void UnbindTexture(unsigned textureId);

		//Material methods.
		 virtual void SetCameraProperties(unsigned meshId, const glm::mat4& worldTransform, TOR::Camera& camera);
		 virtual void SetMaterialProperties(unsigned meshId, PBRMaterial& material);

		//Light properties
		 virtual void SetDirectionalLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, DirectionalLight& light, Material& material);
		 virtual void SetPointlightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, PointLight& light, Material& material);
		 virtual void SetSpotlightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, SpotLight& light, Material& material);

		 virtual void SetMeshProperties(unsigned meshId, const glm::mat4& modelTransform);

		 virtual void AssignShaderToMesh(unsigned meshId, EShaderType type);

		//Actual rendering methods.
		 virtual void ResizeViewport(unsigned viewportWidth, unsigned viewportHeight);
		 virtual void Render(unsigned meshId, unsigned indexCount);
		 virtual void SetMeshShadowProperties(unsigned meshId, const glm::mat4& modelTransform);
		 virtual void SetupShadowPass(const glm::vec3& lightPosition, const glm::vec3& lightDirection);
		 virtual void EndShadowPass();
		 virtual void SetupScenePass();
		 virtual void EndScenePass();

		//Utility.
		SystemStats GetSystemStats();
		glm::mat4x4 tempToAvoidErrors = glm::identity<glm::mat4x4>();
	
	private:
		static LowLevelAPI* s_Renderer;
	};

}