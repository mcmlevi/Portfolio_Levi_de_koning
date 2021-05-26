#pragma once
#include <vector>
#include <CoreRenderAPI/Components/Mesh.h>
#include <CoreRenderAPI/Components/Texture.h>
#include <CoreRenderAPI/Components/Light.h>
#include <CoreRenderAPI/Components/Material.h>
#include <CoreRenderAPI/Components/Transform.h>
#include <CoreRenderAPI/Components/Sampler.h>
#include <CoreRenderAPI/Components/Camera.h>

namespace TOR
{
	/// <summary>
	/// The supported values within the opengl api.
	/// </summary>
	enum class EValueTypes
	{
		BYTE			= 0x1400,
		UNSIGNED_BYTE	= 0x1401,
		SHORT			= 0x1402,
		UNSIGNED_SHORT	= 0x1403,
		INT				= 0x1404,
		UNSIGNED_INT	= 0x1405,
		FLOAT			= 0x1406
	};

	/// <summary>
	/// Draw types used to to draw EBOs in different manners.
	/// </summary>
	enum class EDrawTypes
	{
		STREAM_DRAW		= 0x88E0,
		STREAM_READ		= 0x88E1,
		STREAM_COPY		= 0x88E2,
		STATIC_DRAW		= 0x88E4,
		STATIC_READ		= 0x88E5,
		STATIC_COPY		= 0x88E6,
		DYNAMIC_DRAW	= 0x88E8,
		DYNAMIC_READ	= 0x88E9,
		DYNAMIC_COPY	= 0x88EA
	};

	/// <summary>
	/// The system states are used to actually check/read the performance from the renderer.
	/// </summary>
	struct SystemStats
	{
		float sceneUpdateTime = 0;
	};

	/// <summary>
	/// The low level API is used as the abstract base class for the external graphics API.
	/// </summary>
	class LowLevelAPI
	{
	public:
		//External graphics API creation.
		static LowLevelAPI* CreateCoreAPI(const char* dllName);

		//Renderer core methods.
		virtual void Init(const char* assetDirectory, unsigned viewportWidth, unsigned viewportHeight) = 0;
		virtual void Setting(const char* name, float value) = 0;
		virtual void Render(unsigned meshId, unsigned meshIndeciesCount) = 0;
		virtual void Shutdown() = 0;

		//Shader property methods.
		virtual void SetMeshProperties(unsigned meshId, const glm::mat4& modelTransform) = 0;
		virtual void SetMaterialProperties(unsigned meshId, PBRMaterial& material) = 0;
		virtual void SetCameraProperties(unsigned meshId, const glm::mat4& worldTransform, TOR::Camera& camera) = 0;
		virtual void SetDirectionalLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, DirectionalLight& light, Material& material) = 0;
		virtual void SetPointLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, PointLight& light, Material& material) = 0;
		virtual void SetSpotLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, SpotLight& light, Material& material) = 0;
		virtual void SetMeshShadowProperties(unsigned meshId, const glm::mat4& modelTransform) = 0;

		//Mesh methods.
		virtual void CreateMesh(Mesh& meshStructToFillIn) = 0;
		virtual void BindMeshForRecording(unsigned meshId) = 0;
		virtual void UnbindMesh(unsigned meshId) = 0;
		virtual void BufferElementData(unsigned meshId, uint32_t byteLength, void* bufferStart, EDrawTypes drawType = EDrawTypes::DYNAMIC_DRAW) = 0;
		virtual void BufferAttributeData(unsigned meshId, EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, 
			const void* bufferStart, EValueTypes DataType = EValueTypes::FLOAT, EDrawTypes drawType = EDrawTypes::STATIC_DRAW) = 0;
		virtual void DestroyMesh(unsigned meshId) = 0;

		//Texture methods.
		virtual void CreateTexture(Texture& texture) = 0;
		virtual void LoadTexture(unsigned textureId, const Image& image, unsigned char* source, int sourceCountInElements, const Sampler& sampler, int format) = 0;
		virtual void DestroyTexture(unsigned textureId) = 0;
		virtual void BindTexture(unsigned textureId, unsigned int slot) = 0;
		virtual void UnbindTexture(unsigned textureId) = 0;
		virtual void AssignShaderToMesh(unsigned meshId, EShaderType type) = 0;

		virtual void ResizeViewport(unsigned viewportWidth, unsigned viewportHeight) = 0;
		virtual void SetupShadowPass(const glm::vec3& lightPosition, const glm::vec3& lightDirection) = 0;
		virtual void EndShadowPass() = 0;
		virtual void SetupScenePass() = 0;
		virtual void EndScenePass() = 0;

	private:
		static LowLevelAPI* m_lowLevelRenderer;
	};
}