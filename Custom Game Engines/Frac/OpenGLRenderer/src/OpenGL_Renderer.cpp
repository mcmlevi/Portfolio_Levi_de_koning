#include "OpenGLRenderer/pch.h"
#include "OpenGLRenderer/OpenGL_Renderer.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLSampler.h"

#if defined (PLATFORM_WINDOWS)
#include "../../Externals/GLFW/include/GLFW/glfw3.h"
#endif

namespace TOR
{
	OpenGL_Renderer::OpenGL_Renderer() :
		m_spotLightCount(0),
		m_pointLightCount(0),
		m_SHADOW_WIDTH(2048),
		m_SHADOW_HEIGHT(2048)
	{}

	void OpenGL_Renderer::Init(const char* assetDirectory, unsigned viewportWidth, unsigned viewportHeight)
	{
		m_viewportWidth = viewportWidth;
		m_viewportHeight = viewportHeight;

#if defined (PLATFORM_WINDOWS)
		//Initialize glad again, since 
		if(gladLoadGL() != GL_TRUE)
		{
			printf("Failed to instantiate openGL in OpenGLRenderer\n");
		}
#elif defined (PLATFORM_SWITCH)
		
#endif
#if CONFIG_DEBUG || CONFIG_DEVELOP
		InitDebugGlCallbacks();
#endif

		//Set the render default render settings.
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glFrontFace(GL_CCW);
		//glEnable(GL_MULTISAMPLE);
		glDepthFunc(GL_LESS);
		glEnable(GL_DEPTH_TEST);

		std::string shaderDirectory = std::string(std::string{ assetDirectory } + "Shaders/GLSL/");

		m_shaderMap.insert(std::make_pair(static_cast<int>(EShaderType::Diffuse), std::make_shared<OpenGLShader>(shaderDirectory + "VS/diffuse.vert", shaderDirectory + "PS/diffuse.frag")));
		m_shaderMap.insert(std::make_pair(static_cast<int>(EShaderType::DiffusePBR), std::make_shared<OpenGLShader>(shaderDirectory + "VS/diffusePBR.vert", shaderDirectory + "PS/diffusePBR.frag")));
		m_shaderMap.insert(std::make_pair(static_cast<int>(EShaderType::DiffuseWithNormalMapping), std::make_shared<OpenGLShader>(shaderDirectory + "VS/diffuseWithNormalMap.vert", shaderDirectory + "PS/diffuseWithNormalMap.frag")));
		m_shaderMap.insert(std::make_pair(static_cast<int>(EShaderType::ShadowMapping), std::make_shared<OpenGLShader>(shaderDirectory + "VS/shadowMapPass.vert", shaderDirectory + "PS/shadowMapPass.frag")));

		InitializeShadowRenderPass();
	}

	void OpenGL_Renderer::InitDebugGlCallbacks()
	{
		glDebugMessageCallback(OpenGL_Renderer::DebugCallbackFunc, nullptr);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, nullptr, GL_FALSE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}

	void OpenGL_Renderer::Setting(const char* name, float value)
	{
		
	}

	void OpenGL_Renderer::SetCameraProperties(unsigned meshId, const glm::mat4& worldTransform, TOR::Camera& camera)
	{
		auto shader = m_meshMap[meshId].GetShader();
		glm::mat4x4 projectionViewMatrix = camera.ProjectionMat * glm::inverse(worldTransform);

		shader->GetParameter("u_vpMat")->SetValue(projectionViewMatrix);
		shader->GetParameter("u_CameraPos")->SetValue(glm::vec3(
			worldTransform[3].x,
			worldTransform[3].y,
			worldTransform[3].z
		));
	}

	void OpenGL_Renderer::SetMaterialProperties(unsigned meshId, PBRMaterial& material)
	{
		auto shader = m_meshMap[meshId].GetShader();
		int textureSlot = 0;

		//Albedo map.
		if (material.albedoMap != nullptr)
		{
			m_textureMap[material.albedoMap->tex.Id].Bind(textureSlot);
			shader->GetParameter("u_Material.AlbedoTexture")->SetValue(m_textureMap[material.albedoMap->tex.Id]);
			shader->GetParameter("u_hasAlbedoTexture")->SetValue(true);

			material.Properties.TexCoords[static_cast<int>(ETextureType::ALBEDO)]; // #TODO: see if we need this and how we are going to use it with other maps.
			textureSlot++;
		}
		else 
		{

			shader->GetParameter("u_hasAlbedoTexture")->SetValue(false);
			shader->GetParameter("u_Material.AlbedoFactor")->SetValue(material.Properties.AlbedoFactor);
		}

		//Normal map.
		if (material.normalMap != nullptr)
		{
			textureSlot = 1;
			m_textureMap[material.normalMap->tex.Id].Bind(textureSlot);
			shader->GetParameter("u_Material.NormalTexture")->SetValue(m_textureMap[material.normalMap->tex.Id]);
		}

		//Metal roughness map.
		if (material.mraoMap != nullptr)
		{
			textureSlot = 2;
			m_textureMap[material.mraoMap->tex.Id].Bind(textureSlot);
			shader->GetParameter("u_Material.AOMROTexture")->SetValue(m_textureMap[material.mraoMap->tex.Id]);
			shader->GetParameter("u_hasRoughnessMetalnessTexture")->SetValue(true);
		}
		else {
			shader->GetParameter("u_hasRoughnessMetalnessTexture")->SetValue(false);
			shader->GetParameter("u_Material.Metalness")->SetValue(material.Properties.MetallicFactor);
			shader->GetParameter("u_Material.Roughness")->SetValue(material.Properties.RoughnessFactor);
		}

		//Emission.
		if (material.emissionMap != nullptr)
		{
			textureSlot = 3;
			m_textureMap[material.emissionMap->tex.Id].Bind(textureSlot);
			shader->GetParameter("u_Material.EmissionFactor")->SetValue(material.Properties.EmissionFactor);
			shader->GetParameter("u_Material.EmissionTexture")->SetValue(m_textureMap[material.emissionMap->tex.Id]);
			shader->GetParameter("u_HasEmissiveTexture")->SetValue(true);
		}
		else
		{
			shader->GetParameter("u_HasEmissiveTexture")->SetValue(false);
		}

		//Set the ambient occlusion.
		shader->GetParameter("u_UseBakedAmbientOcclusion")->SetValue(material.Properties.HasOcclusionMap);
		shader->GetParameter("u_FlipNormals")->SetValue(material.Properties.FlipNormals);

		//Shadowmap.
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
		shader->GetParameter("u_Material.ShadowTexture")->SetValue(4);
		shader->GetParameter("u_lightSpaceMatrix")->SetValue(m_lightViewMatrix);
	}

	void OpenGL_Renderer::SetDirectionalLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, DirectionalLight& light, Material& material)
	{
		auto shader = m_meshMap[meshId].GetShader();

		shader->GetParameter("u_DirectionalLightDir")->SetValue(glm::vec3(lightWorldMatrix[2]));
		shader->GetParameter("u_DirectionalLight.Color")->SetValue(light.Color);
		shader->GetParameter("u_DirectionalLight.Intensity")->SetValue(light.Intensity);
		shader->GetParameter("u_HasDirectionalLight")->SetValue(true);
	}

	void OpenGL_Renderer::SetPointLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, PointLight& light, Material& material)
	{
		auto shader = m_meshMap[meshId].GetShader();

		PointLight& pointLight = static_cast<PointLight&>(light);
		shader->GetParameter("u_PointLightPositions[" + std::to_string(m_pointLightCount) + "].Position")->SetValue(glm::vec3(lightWorldMatrix[3]));
		shader->GetParameter("u_PointLights[" + std::to_string(m_pointLightCount) + "].Intensity")->SetValue(pointLight.Intensity);
		shader->GetParameter("u_PointLights[" + std::to_string(m_pointLightCount) + "].Color")->SetValue(pointLight.Color);
		shader->GetParameter("u_PointLights[" + std::to_string(m_pointLightCount) + "].Range")->SetValue(pointLight.Radius);
		++m_pointLightCount;

		shader->GetParameter("u_PointLightCount")->SetValue(m_pointLightCount);
	}

	void OpenGL_Renderer::SetSpotLightProperties(unsigned meshId, const glm::mat4& lightWorldMatrix, SpotLight& light, Material& material)
	{
		++m_spotLightCount;
		printf("WARNING: Spotlights are not supported in the renderer yet!\n");
	}

	void OpenGL_Renderer::SetMeshProperties(unsigned meshId, const glm::mat4& modelTransform)
	{
		auto shader = m_meshMap[meshId].GetShader();
		shader->Activate();
		shader->GetParameter("u_modelMat")->SetValue(modelTransform);
		shader->GetParameter("u_normalMat")->SetValue(glm::transpose(glm::inverse(glm::mat3(modelTransform))));
	}

	void OpenGL_Renderer::Render(unsigned meshId, unsigned meshIndeciesCount)
	{
		m_meshMap[meshId].Draw(meshIndeciesCount);
		m_pointLightCount = 0; //#TODO: Implement a setup renderpass method, that handles the counters for these.
		m_spotLightCount = 0;
	}

	void OpenGL_Renderer::CreateMesh(Mesh& meshStructToFillIn)
	{
		static int MeshIdCounter = 0;
		OpenGLMesh mesh;
		mesh.Initialize();
		m_meshMap.insert(std::make_pair(MeshIdCounter, mesh));
		meshStructToFillIn.Id = MeshIdCounter;
		++MeshIdCounter;
	}

	void OpenGL_Renderer::DestroyMesh(unsigned meshId)
	{
		TOR_ASSERT(m_meshMap.find(meshId) == m_meshMap.end());
		m_meshMap.erase(meshId);
	}

	void OpenGL_Renderer::BindMeshForRecording(unsigned meshId)
	{
		m_meshMap[meshId].Bind();
	}

	void OpenGL_Renderer::UnbindMesh(unsigned meshId)
	{
		m_meshMap[meshId].Unbind();
	}

	void OpenGL_Renderer::BufferAttributeData(unsigned meshId, EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, EValueTypes DataType /*= EValueTypes::FLOAT*/, EDrawTypes DrawMode /*= EDrawTypes::STATIC_DRAW*/)
	{
		unsigned long glDataType = ParseValueTypeToGL(DataType);
		unsigned long glDrawMode = ParseBufferTypeToGL(DrawMode);
		m_meshMap[meshId].BufferAttributeData(attrType, byteLength, ByteStride, attrElementCount, bufferStart, glDataType, glDrawMode);
	}

	void OpenGL_Renderer::BufferElementData(unsigned meshId, uint32_t byteLength, void* bufferStart, EDrawTypes DrawMode /*= EDrawTypes::STATIC_DRAW*/)
	{
		unsigned long glDrawMode = ParseBufferTypeToGL(DrawMode);
		m_meshMap[meshId].BufferElementData(byteLength, bufferStart, glDrawMode);
	}

	void OpenGL_Renderer::CreateTexture(Texture& texture)
	{
		OpenGLTexture glTexture;
		static int textureIdCounter = 0;
		m_textureMap.insert(std::make_pair(textureIdCounter, glTexture));
		texture.Id = textureIdCounter;
		++textureIdCounter;
	}

	void OpenGL_Renderer::LoadTexture(unsigned textureId, const Image& image, unsigned char* source, int sourceCountInElements, const Sampler& sampler, int format)
	{
		OpenGLSampler glSampler = ParseSamplerToGLSampler(sampler);
		m_textureMap[textureId].Initialize(image, glSampler, source, sourceCountInElements, format);
	}

	void OpenGL_Renderer::DestroyTexture(unsigned textureId)
	{
		TOR_ASSERT(m_textureMap.find(textureId) != m_textureMap.end());
		m_textureMap.erase(textureId);
	}

	void OpenGL_Renderer::BindTexture(unsigned textureId, unsigned int slot)
	{
		m_textureMap[textureId].Bind(slot);
	}

	void OpenGL_Renderer::UnbindTexture(unsigned textureId)
	{
		m_textureMap[textureId].Unbind();
	}

	void OpenGL_Renderer::AssignShaderToMesh(unsigned meshId, EShaderType type)
	{
		m_meshMap[meshId].SetShader(m_shaderMap[static_cast<int>(type)]);
	}

	void OpenGL_Renderer::SetupShadowPass(const glm::vec3& lightPosition, const glm::vec3& lightDirection)
	{
		float nearPlane = 100.0f, farPlane = 160.0f;
		glm::mat4 lightProjection = glm::orthoRH(-20.0f, 15.0f, -20.0f, 15.0f, nearPlane, farPlane);
		glm::mat4 lookAtMat = glm::lookAtRH(lightPosition, glm::vec3(20.0f, 2.0f, -15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		m_lightViewMatrix = lightProjection * lookAtMat;

		m_shaderMap[static_cast<int>(EShaderType::ShadowMapping)]->Activate();
		m_shaderMap[static_cast<int>(EShaderType::ShadowMapping)]->GetParameter("u_lightSpaceMatrix")->SetValue(m_lightViewMatrix, false);

		glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL_Renderer::SetMeshShadowProperties(unsigned meshId, const glm::mat4& modelTransform)
	{
		m_shaderMap[static_cast<int>(EShaderType::ShadowMapping)]->GetParameter("u_modelMat")->SetValue(modelTransform);
	}

	void OpenGL_Renderer::EndShadowPass()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if CONFIG_DEBUG || CONFIG_DEVELOP
		m_shaderMap[static_cast<int>(EShaderType::ShadowMapping)]->Deactivate();
#endif
	}

	void OpenGL_Renderer::SetupScenePass()
	{
		//m_shaderMap[static_cast<int>(EShaderType::DiffusePBR)]->Activate();
		glViewport(0, 0, m_viewportWidth, m_viewportHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL_Renderer::EndScenePass()
	{
#if CONFIG_DEBUG || CONFIG_DEVELOP
		m_shaderMap[static_cast<int>(EShaderType::DiffusePBR)]->Deactivate();
#endif
	}

	void OpenGL_Renderer::ResizeViewport(unsigned width, unsigned height)
	{
		m_viewportWidth = width;
		m_viewportHeight = height;
	}

	unsigned long OpenGL_Renderer::ParseValueTypeToGL(EValueTypes valueType)
	{
		switch(valueType)
		{
		case EValueTypes::BYTE:
			return GL_BYTE;
		case EValueTypes::UNSIGNED_BYTE:
			return GL_UNSIGNED_BYTE;
		case EValueTypes::UNSIGNED_SHORT:
			return GL_UNSIGNED_SHORT;
		case EValueTypes::INT:
			return GL_INT;
		case EValueTypes::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		case EValueTypes::FLOAT:
			return GL_FLOAT;
		default:
			printf("ERROR: Unspecified value type: %i\n.", static_cast<int>(valueType));
		}

		assert(false);
		return 0;
	}

	unsigned long OpenGL_Renderer::ParseBufferTypeToGL(EDrawTypes drawTypes)
	{
		switch(drawTypes)
		{
		case EDrawTypes::STREAM_DRAW:
			return GL_STREAM_DRAW;
		case EDrawTypes::STREAM_READ:
			return GL_STREAM_READ;
		case EDrawTypes::STREAM_COPY:
			return GL_STREAM_COPY;
		case EDrawTypes::STATIC_DRAW:
			return GL_STATIC_DRAW;
		case EDrawTypes::STATIC_READ:
			return GL_STATIC_READ;
		case EDrawTypes::STATIC_COPY:
			return GL_STATIC_COPY;
		case EDrawTypes::DYNAMIC_DRAW:
			return GL_DYNAMIC_DRAW;
		case EDrawTypes::DYNAMIC_READ:
			return GL_DYNAMIC_READ;
		case EDrawTypes::DYNAMIC_COPY:
			return GL_DYNAMIC_COPY;
		default:
			printf("ERROR: Unspecified value type: %i\n.", static_cast<int>(drawTypes));
		}

		assert(false);
		return 0;
	}

	void OpenGL_Renderer::InitializeShadowRenderPass()
	{
		glGenFramebuffers(1, &m_depthMapFBO);

		glGenTextures(1, &m_depthTextureId);
		glBindTexture(GL_TEXTURE_2D, m_depthTextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_SHADOW_WIDTH, m_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

		glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTextureId, 0);
		glDrawBuffer(GL_NONE); //We're not rendering color data.
		glReadBuffer(GL_NONE); //We're not rendering color data.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGL_Renderer::Shutdown()
	{
		
	}

#if defined (PLATFORM_WINDOWS)
	void APIENTRY
#elif defined (PLATFORM_SWITCH)
	
#endif
	 OpenGL_Renderer::DebugCallbackFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam)
	{
		//Exclude some logs on release.
#if defined(CONFIG_RELEASE)
		switch (id)
		{
			//A debug context shouldn't exist on release.
		case 131218:	// http://stackoverflow.com/questions/12004396/opengl-debug-context-performance-warning 
			return;
		}
#endif
		std::string sourceString;
		std::string typeString;
		std::string severityString;

		switch (source)
		{
		case GL_DEBUG_SOURCE_API:
		{
			sourceString = "API";
			break;
		}
		case GL_DEBUG_SOURCE_APPLICATION:
		{
			sourceString = "Application";
			break;
		}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		{
			sourceString = "Window System";
			break;
		}
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
		{
			sourceString = "Shader Compiler";
			break;
		}
		case GL_DEBUG_SOURCE_THIRD_PARTY:
		{
			sourceString = "Third Party";
			break;
		}
		case GL_DEBUG_SOURCE_OTHER:
		{
			sourceString = "Other";
			break;
		}
		default:
		{
			sourceString = "Unknown";
			break;
		}
		}

		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR:
		{
			typeString = "Error";
			break;
		}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		{
			typeString = "Deprecated Behavior";
			break;
		}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		{
			typeString = "Undefined Behavior";
			break;
		}
		case GL_DEBUG_TYPE_PERFORMANCE:
		{
			typeString = "Performance";
			break;
		}
		case GL_DEBUG_TYPE_OTHER:
		{
			typeString = "Other";
			break;
		}
		default: {
			typeString = "Unknown";
			break;
		}
		}

		switch (severity)
		{
		case GL_DEBUG_SEVERITY_HIGH:
		{
			severityString = "High";
			break;
		}
		case GL_DEBUG_SEVERITY_MEDIUM:
		{
			severityString = "Medium";
			break;
		}
		case GL_DEBUG_SEVERITY_LOW:
		{
			severityString = "Low";
			break;
		}
		default:
		{
			severityString = "Unknown";
			return;
		}
		}

		spdlog::error("GL Debug Callback:");
		spdlog::error("  source:       {source}:{sourceString}", fmt::arg("source", source), fmt::arg("sourceString", sourceString.c_str()));
		spdlog::error("  type:         {source}:{typeString}", fmt::arg("source", type), fmt::arg("typeString", typeString.c_str()));
		spdlog::error("  id:           {id}", fmt::arg("id", id));
		spdlog::error("  severity:     {severity}:{severityString}", fmt::arg("severity", severity), fmt::arg("severityString", severityString.c_str()));
		spdlog::error("  message:      {message}", fmt::arg("message", message));
		spdlog::error("------------------------------------------------------------------");
		TOR_ASSERT(type != GL_DEBUG_TYPE_ERROR);
	}
}