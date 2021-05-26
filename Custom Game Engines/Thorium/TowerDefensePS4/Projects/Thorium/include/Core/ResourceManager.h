#pragma once
#include <unordered_map>
#include <string>
#include <Components/ModelComponent.h>
#include <Resources/EModelFileType.h>
#include <Graphics/ResourceLoader.h>
#include <Resources/Shader.h>

namespace Th
{
	struct BoxColliderComponent;
	/// <summary>
	/// Cross platform resource management.
	/// </summary>
	class ResourceManager
	{
	public:
		static ResourceManager& GetInstance();
		
		void Initialize();
		void Destroy();

		std::string LoadModel(ResourceLoader& resourceLoader, const std::string& filePath, EModelFileType modelType);
		void UnloadModel(const std::string& key);

		std::string LoadShader(ResourceLoader& resourceLoader, const std::string& vertShader, const std::string& fragShader);
		void UnloadShader(const std::string& key);

#if PLATFORM_PS4
		std::string LoadCsShader(ResourceLoader& resourceLoader, const std::string& shaderPath);
		std::string LoadPsShader(ResourceLoader& resourceLoader, const std::string& shaderPath);
		std::string LoadVsShader(ResourceLoader& resourceLoader, const std::string& shaderPath);

		EmbeddedCsShader* GetCsShader(const std::string& key);
		EmbeddedVsShader* GetVsShader(const std::string& key);
		EmbeddedPsShader* GetPsShader(const std::string& key);
#endif
		Components::ModelComponent* GetModel(const std::string& key);
		Shader* GetShader(const std::string& key);

		/**
		 * \brief Adds a wildcard to the resource manager to make loading resources a bit easier for example [test]a.obj = filepath/models/test/a.obj
		 * \param wildcard the wildcard [test] for example but can be anything
		 * \param value the value is should evaluate to for example filepath/models/test/
		 */
		void AddWildCard(const std::string& wildcard, const std::string& value);
		/**
		 * \brief Removes a wildcard from the list of added wildcards
		 * \param wildcard the wildcard to remove
		 */
		void RemoveWildCard(const std::string& wildcard);

	private:
		std::string EvaluateWildCard(const std::string& filepath);

		ResourceManager() = default;
		~ResourceManager() = default;
		
		std::unordered_map < std::string, Components::ModelComponent* > m_ModelMap;
		std::unordered_map < std::string, Shader* > m_ShaderMap;
		std::unordered_map<std::string, BoxColliderComponent* > m_BoxColliderMap;
		std::unordered_map < std::string, std::string > m_wildcardMap;

#if PLATFORM_PS4
		MemoryArea m_OnionAllocator;
		MemoryArea m_GarlicAllocator;

		std::unordered_map < std::string, EmbeddedVsShader* > m_VsMap;
		std::unordered_map < std::string, EmbeddedCsShader* > m_CsMap;
		std::unordered_map < std::string, EmbeddedPsShader* > m_PsMap;
#endif
	};
}