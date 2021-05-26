#include "CoreRenderAPI/pch.h"
#include "CoreRenderAPI/RenderAPI.h"

namespace TOR
{
	LowLevelAPI* RenderAPI::s_Renderer = nullptr;
	static RenderAPI HighLevelRenderAPI;
	
	/// <summary>
	/// If internal rendering system is not made yet, make new RenderSystem, init low level API from dll.
	/// </summary>
	/// <param name="dllName">Name of dll file that contains low level rendering. Use "OpenGLRenderer" for now.</param>
	/// <returns> pointer to high level API</returns>
	RenderAPI* RenderAPI::CreateRenderAPI(const char* dllName)
	{
		if (!s_Renderer)
		{
			s_Renderer = LowLevelAPI::CreateCoreAPI(dllName);
		}
		return &HighLevelRenderAPI;
	}
	
	void RenderAPI::Initialize(const std::string& assetDirectory, unsigned viewportWidth, unsigned viewportHeight)
	{
		s_Renderer->Init(assetDirectory.c_str(), viewportWidth, viewportHeight);
	}

	void RenderAPI::Shutdown()
	{
		s_Renderer->Shutdown();
	}

	void RenderAPI::CreateMesh(Mesh& meshStructToFill)
	{
		s_Renderer->CreateMesh(meshStructToFill);
	}

	void RenderAPI::DestroyMesh(unsigned meshId)
	{
		s_Renderer->DestroyMesh(meshId);
	}

	void RenderAPI::BindMeshForRecording(unsigned meshId)
	{
		s_Renderer->BindMeshForRecording(meshId);
	}

	void RenderAPI::UnbindMesh(unsigned meshId)
	{
		s_Renderer->UnbindMesh(meshId);
	}

	void RenderAPI::BufferAttributeData(unsigned meshId, EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, EValueTypes DataType, EDrawTypes DrawMode)
	{
		s_Renderer->BufferAttributeData(meshId, attrType, byteLength, ByteStride, attrElementCount, bufferStart, DataType, DrawMode);
	}

	void RenderAPI::BufferElementData(unsigned meshId, uint32_t byteLength, void* bufferStart, EDrawTypes DrawMode)
	{
		s_Renderer->BufferElementData(meshId, byteLength, bufferStart, DrawMode);
	}
	
	void RenderAPI::CreateTexture(Texture& texture)
	{
		s_Renderer->CreateTexture(texture);
	}

	void RenderAPI::LoadTexture(unsigned textureId, const Image& image, unsigned char* source, int sourceCountInElements, const Sampler& sampler, int format)
	{
		s_Renderer->LoadTexture(textureId, image, source, sourceCountInElements, sampler, format);
	}

	void RenderAPI::DestroyTexture(unsigned textureId)
	{
		s_Renderer->DestroyTexture(textureId);
	}

	void RenderAPI::BindTexture(unsigned textureId, unsigned int slot)
	{
		s_Renderer->BindTexture(textureId, slot);
	}

	void RenderAPI::UnbindTexture(unsigned textureId)
	{
		s_Renderer->UnbindTexture(textureId);
	}

	void RenderAPI::SetCameraProperties(unsigned meshId, const glm::mat4& worldTransform, TOR::Camera& camera)
	{
		s_Renderer->SetCameraProperties(meshId, worldTransform, camera);
	}

	void RenderAPI::SetMaterialProperties(unsigned meshId, PBRMaterial& material)
	{
		s_Renderer->SetMaterialProperties(meshId, material);
	}

	void RenderAPI::SetDirectionalLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, DirectionalLight& light, Material& material)
	{
		s_Renderer->SetDirectionalLightProperties(meshId, lightWorldMatrix, light, material);
	}

	void RenderAPI::SetPointlightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, PointLight& light, Material& material)
	{
		s_Renderer->SetPointLightProperties(meshId, lightWorldMatrix, light, material);
	}

	void RenderAPI::SetSpotlightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, SpotLight& light, Material& material)
	{
		s_Renderer->SetSpotLightProperties(meshId, lightWorldMatrix, light, material);
	}

	void RenderAPI::SetMeshProperties(unsigned meshId, const glm::mat4& modelTransform)
	{
		s_Renderer->SetMeshProperties(meshId, modelTransform);
	}

	void RenderAPI::AssignShaderToMesh(unsigned meshId, EShaderType type)
	{
		s_Renderer->AssignShaderToMesh(meshId,type);
	}

	void RenderAPI::ResizeViewport(unsigned viewportWidth, unsigned viewportHeight)
	{
		s_Renderer->ResizeViewport(viewportWidth, viewportHeight);
	}

	void RenderAPI::Render(unsigned meshId, unsigned indexCount)
	{
		s_Renderer->Render(meshId, indexCount);
	}

	void RenderAPI::SetMeshShadowProperties(unsigned meshId, const glm::mat4& modelTransform)
	{
		s_Renderer->SetMeshShadowProperties(meshId, modelTransform);
	}

	void RenderAPI::SetupShadowPass(const glm::vec3& lightPosition, const glm::vec3& lightDirection)
	{
		s_Renderer->SetupShadowPass(lightPosition, lightDirection);
	}

	void RenderAPI::EndShadowPass()
	{
		s_Renderer->EndShadowPass();
	}

	void RenderAPI::SetupScenePass()
	{
		s_Renderer->SetupScenePass();
	}

	void RenderAPI::EndScenePass()
	{
		s_Renderer->EndScenePass();
	}

	TOR::SystemStats RenderAPI::GetSystemStats()
	{
		return TOR::SystemStats{};
	}
}