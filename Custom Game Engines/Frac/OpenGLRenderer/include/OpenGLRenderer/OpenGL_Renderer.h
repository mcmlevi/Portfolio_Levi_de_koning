#pragma once
#include <CoreRenderAPI/LowLevelAPI.h>
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLMesh.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLTexture.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLShader.h"

#if defined(PLATFORM_SWITCH)

#elif defined(PLATFORM_WINDOWS)
#ifdef TOR_API_EXPORTS
#define TOR_API __declspec(dllexport)
#else
#define TOR_API __declspec(dllimport)
#endif
#endif

namespace TOR
{
	/// <summary>
	/// Used to identify the type of renderpasses.
	/// </summary>
	enum class ERenderPassType
	{
		SHADOW,
		SCENE
	};

	/// <summary>
	/// The render settings of the renderer.
	/// </summary>
	struct RenderSettings
	{
		
	};

	struct Mesh;
	struct Texture;
	struct Camera;
	struct DirectionalLight;

	/// <summary>
	/// The OpenGL renderer implements the low level API functionality, thereby making sure that models can be rendered.
	/// </summary>
	class OpenGL_Renderer : public LowLevelAPI
	{
	public:
		OpenGL_Renderer();
		~OpenGL_Renderer() = default;

		void Init(const char* assetDirectory, unsigned viewportWidth, unsigned viewportHeight) override;
		void Shutdown() override;

		void InitDebugGlCallbacks();
		void Setting(const char* name, float value) override;

		void Render(unsigned meshId, unsigned meshIndeciesCount) override;

		void SetCameraProperties(unsigned meshId, const glm::mat4& worldTransform, TOR::Camera& camera) override;
		void SetMaterialProperties(unsigned meshId, PBRMaterial& material) override;
		void SetDirectionalLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, DirectionalLight& light, Material& material) override;
		void SetPointLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, PointLight& light, Material& material) override;
		void SetSpotLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, SpotLight& light, Material& material) override;
		void SetMeshProperties(unsigned meshId, const glm::mat4& modelTransform) override;
		void SetMeshShadowProperties(unsigned meshId, const glm::mat4& modelTransform) override;

		void CreateMesh(Mesh& meshStructToFillIn) override;
		void DestroyMesh(unsigned meshId) override;
		void BindMeshForRecording(unsigned meshId) override;
		void UnbindMesh(unsigned meshId) override;
		void BufferAttributeData(unsigned meshId, EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, EValueTypes DataType = EValueTypes::FLOAT, EDrawTypes DrawMode = EDrawTypes::STATIC_DRAW) override;
		void BufferElementData(unsigned meshId, uint32_t byteLength, void* bufferStart, EDrawTypes DrawMode = EDrawTypes::STATIC_DRAW) override;

		void CreateTexture(Texture& texture) override;
		void LoadTexture(unsigned textureId, const Image& image, unsigned char* source, int sourceCountInElements, const Sampler& sampler, int format) override;
		void DestroyTexture(unsigned textureId) override;
		void BindTexture(unsigned textureId, unsigned int slot) override;
		void UnbindTexture(unsigned textureId) override;

		void AssignShaderToMesh(unsigned meshId, EShaderType type) override;

		void SetupShadowPass(const glm::vec3& lightPosition, const glm::vec3& lightDirection) override;
		void EndShadowPass() override;
		void SetupScenePass() override;
		void EndScenePass() override;

		void ResizeViewport(unsigned width, unsigned height) override;

	private:
		void InitializeShadowRenderPass();

#if defined (PLATFORM_WINDOWS)
		static void APIENTRY
#elif defined (PLATFORM_SWITCH)
		
#endif
		DebugCallbackFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam);

		std::unordered_map<int, OpenGLMesh> m_meshMap;
		std::unordered_map<int, OpenGLTexture> m_textureMap;
		std::unordered_map<int, std::shared_ptr<OpenGLShader>> m_shaderMap;

		int m_pointLightCount;
		int m_spotLightCount;

		unsigned m_depthMapFBO;
		unsigned m_depthTextureId;
		const unsigned m_SHADOW_WIDTH, m_SHADOW_HEIGHT;
		unsigned m_viewportWidth, m_viewportHeight;

		unsigned long ParseValueTypeToGL(EValueTypes valueType);
		unsigned long ParseBufferTypeToGL(EDrawTypes drawTypes);

		glm::mat4 m_lightViewMatrix = glm::mat4(1.0f);
	};
}