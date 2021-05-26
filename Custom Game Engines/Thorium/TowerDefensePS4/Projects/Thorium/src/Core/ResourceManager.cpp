#include <Thpch.h>
#include <Core/ResourceManager.h>
#include <Core/MessageLogger.h>
#include <Core/WorldManager.h>
#include <Graphics/ModelLoading/ObjFactory.h>
#include <Graphics/ModelLoading/GLTFFactory.h>
#include <Components/ModelComponent.h>
#include <Components/TransformComponent.h>
#include <Graphics/ResourceLoader.h>

namespace Th
{
	ResourceManager& Th::ResourceManager::GetInstance()
	{
		static ResourceManager manager{};
		return manager;
	}

	void ResourceManager::Initialize()
	{
#if PLATFORM_PS4
		m_OnionAllocator = MemoryManager::GetInstance().CreateMemoryAllocator<LinearAllocatorBehavior>(
			16 * 1024 * 1024,
			SceKernelMemoryType::SCE_KERNEL_WB_ONION,
			SCE_KERNEL_PROT_CPU_RW | SCE_KERNEL_PROT_GPU_ALL);
		m_GarlicAllocator = MemoryManager::GetInstance().CreateMemoryAllocator<LinearAllocatorBehavior>(
			64 * 4 * 1024 * 1024,
			SceKernelMemoryType::SCE_KERNEL_WC_GARLIC,
			SCE_KERNEL_PROT_CPU_WRITE | SCE_KERNEL_PROT_GPU_ALL);
#endif
	}

	void ResourceManager::Destroy()
	{
#if PLATFORM_PS4
		MemoryManager::GetInstance().Reset(m_OnionAllocator);
		MemoryManager::GetInstance().Reset(m_GarlicAllocator);
#endif
	}

	std::string ResourceManager::LoadModel(ResourceLoader& resourceLoader, const std::string& filePath, EModelFileType modelType)
	{
		std::string evaluatedFilePath = EvaluateWildCard(filePath);
		if(m_ModelMap.find(evaluatedFilePath) != m_ModelMap.end())
		{
			return evaluatedFilePath;
		}

#if PLATFORM_WIN64
		m_ModelMap.emplace(evaluatedFilePath, resourceLoader.LoadModel(evaluatedFilePath, modelType));
#elif PLATFORM_PS4
		m_ModelMap.emplace(evaluatedFilePath, resourceLoader.LoadModel(evaluatedFilePath, modelType, m_OnionAllocator, m_GarlicAllocator));
#endif
		return evaluatedFilePath;
	}

	void ResourceManager::UnloadModel(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if(m_ModelMap.find(evaluatedFilePath) != m_ModelMap.end())
		{
			m_ModelMap.erase(evaluatedFilePath);
		}
		else
		{
			LOGERROR("The model you are trying to remove doesn't exist");
		}
	}

#if PLATFORM_WIN64
	std::string ResourceManager::LoadShader(ResourceLoader& resourceLoader, const std::string& vertShader, const std::string& fragShader)
	{
		//TODO: Find a way to store the shader filepath appropriately.
		std::string vertFilePath = EvaluateWildCard(vertShader);
		std::string fragFilePath = EvaluateWildCard(fragShader);

		if (m_ModelMap.find(vertFilePath) != m_ModelMap.end())
		{
			return vertFilePath;
		}

		m_ShaderMap.emplace(vertFilePath, resourceLoader.LoadShaderPipeline(vertFilePath, fragFilePath));
		return vertShader;
	}

#elif PLATFORM_PS4

	std::string ResourceManager::LoadShader(ResourceLoader& resourceLoader, const std::string& vertShader, const std::string& fragShader)
	{
		//TODO: Find a way to store the shader filepath appropriately.
		std::string vertFilePath = EvaluateWildCard(vertShader);
		std::string fragFilePath = EvaluateWildCard(fragShader);

		if (m_ShaderMap.find(vertFilePath) != m_ShaderMap.end())
		{
			return vertFilePath;
		}

		std::string vsShaderName = LoadVsShader(resourceLoader, vertFilePath);
		std::string fsShaderName = LoadPsShader(resourceLoader, fragFilePath);

		Shader* shader = new Shader();
		shader->Initialize(vertFilePath.c_str(), GetVsShader(vsShaderName), GetPsShader(vsShaderName));
		m_ShaderMap.emplace(vertShader, shader);
		
		return vertShader;
	}

	std::string ResourceManager::LoadCsShader(ResourceLoader& resourceLoader, const std::string& shaderPath)
	{
		std::string evaluatedFilePath = EvaluateWildCard(shaderPath);
		if (m_CsMap.find(evaluatedFilePath) != m_CsMap.end())
		{
			return evaluatedFilePath;
		}

		EmbeddedCsShader* embeddedCsShader = new EmbeddedCsShader(resourceLoader.LoadShaderFromFile<sce::Gnmx::CsShader>(evaluatedFilePath.c_str(), m_OnionAllocator, m_GarlicAllocator).embedded);
		m_CsMap.emplace(evaluatedFilePath, embeddedCsShader);		
		return evaluatedFilePath;
	}

	std::string ResourceManager::LoadPsShader(ResourceLoader& resourceLoader, const std::string& shaderPath)
	{
		std::string evaluatedFilePath = EvaluateWildCard(shaderPath);
		if (m_PsMap.find(evaluatedFilePath) != m_PsMap.end())
		{
			return evaluatedFilePath;
		}

		EmbeddedPsShader* embeddedPsShader = new EmbeddedPsShader(resourceLoader.LoadShaderFromFile<sce::Gnmx::PsShader>(evaluatedFilePath.c_str(), m_OnionAllocator, m_GarlicAllocator).embedded);
		m_PsMap.emplace(evaluatedFilePath, embeddedPsShader);
		return evaluatedFilePath;
	}

	std::string ResourceManager::LoadVsShader(ResourceLoader& resourceLoader, const std::string& shaderPath)
	{
		std::string evaluatedFilePath = EvaluateWildCard(shaderPath);
		if (m_VsMap.find(evaluatedFilePath) != m_VsMap.end())
		{
			return evaluatedFilePath;
		}

		EmbeddedVsShader* embeddedvsShader = new EmbeddedVsShader(resourceLoader.LoadShaderFromFile<sce::Gnmx::VsShader>(evaluatedFilePath.c_str(), m_OnionAllocator, m_GarlicAllocator).embedded);
		m_VsMap.emplace(evaluatedFilePath, embeddedvsShader);
		return evaluatedFilePath;
	}

	EmbeddedCsShader* ResourceManager::GetCsShader(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_CsMap.find(evaluatedFilePath) != m_CsMap.end())
		{
			return m_CsMap[evaluatedFilePath];
		}
		else
		{
			LOGERROR("The shader:%s, you are trying to get doesn't exist", evaluatedFilePath.c_str());
			return nullptr;
		}
	}

	EmbeddedVsShader* ResourceManager::GetVsShader(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_VsMap.find(evaluatedFilePath) != m_VsMap.end())
		{
			return m_VsMap[evaluatedFilePath];
		}
		else
		{
			LOGERROR("The shader:%s, you are trying to get doesn't exist", evaluatedFilePath.c_str());
			return nullptr;
		}
	}

	EmbeddedPsShader* ResourceManager::GetPsShader(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_PsMap.find(evaluatedFilePath) != m_PsMap.end())
		{
			return m_PsMap[evaluatedFilePath];
		}
		else
		{
			LOGERROR("The shader:%s, you are trying to get doesn't exist", evaluatedFilePath.c_str());
			return nullptr;
		}
	}

#endif

	void ResourceManager::UnloadShader(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_ShaderMap.find(evaluatedFilePath) != m_ShaderMap.end())
		{
			m_ShaderMap.erase(evaluatedFilePath);
		}
		else
		{
			LOGERROR("The shader you are trying to remove doesn't exist");
		}
	}

	Components::ModelComponent* Th::ResourceManager::GetModel(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_ModelMap.find(evaluatedFilePath) != m_ModelMap.end())
		{
			return m_ModelMap[evaluatedFilePath];
		}
		else
		{
			LOGERROR("The model you are trying to get doesn't exist");
		}

		return nullptr;
	}

	Shader* ResourceManager::GetShader(const std::string& key)
	{
		std::string evaluatedFilePath = EvaluateWildCard(key);
		if (m_ShaderMap.find(evaluatedFilePath) != m_ShaderMap.end())
		{
			return m_ShaderMap[evaluatedFilePath];
		}
		else
		{
			LOGERROR("The shader:%s, you are trying to get doesn't exist", evaluatedFilePath.c_str());
			return nullptr;
		}
	}

	void ResourceManager::AddWildCard(const std::string& wildcard, const std::string& value)
	{
		if(m_wildcardMap.find(wildcard) != m_wildcardMap.end())
		{
			m_wildcardMap[wildcard] = value;
		}else
		{
			m_wildcardMap.insert(std::make_pair(wildcard, value));
		}
	}

	void ResourceManager::RemoveWildCard(const std::string& wildcard)
	{
		if(m_wildcardMap.find(wildcard) != m_wildcardMap.end())
		{
			m_wildcardMap.erase(wildcard);
		}else
		{
			printf("Wild card does not exist!\n");
		}
	}

	std::string ResourceManager::EvaluateWildCard(const std::string& filepath)
	{
		std::string parsedString{filepath};
	
		auto it = m_wildcardMap.begin();
		while (it != m_wildcardMap.end())
		{
			int found = parsedString.find(it->first);
			if(found != std::string::npos)
			{
				parsedString.replace(parsedString.begin() + found,parsedString.begin() + it->first.size(), it->second);
			}
			it++;
		}
		return parsedString;
	}
}
