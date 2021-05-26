#pragma once

#include "Core/ECSWrapper.h"


namespace Frac 
{

	struct PrefabbedResource {
		int refCount = 0;
		std::string resourceName;
		EntityHandle entityWithResources;
	};

	struct SubResourceContainer {

		enum class TypeOfSubresource {
			CHILD,
			ATTACHEMENT
		};

		std::vector<std::pair<std::string,TypeOfSubresource>> prefabResources;
	};

	class RenderSystem;

	class PrefabManager
	{
	public:
		PrefabManager(RenderSystem& a_renderingSystem, ECSWrapper& a_entityManager);
		~PrefabManager() = default;

		virtual bool HasPrefab(std::string a_prefabName);
		virtual EntityHandle GetPrefab(std::string a_prefabName);
		virtual EntityHandle GetPrefabInstance(std::string a_prefabName);
		virtual void CachePrefab(std::string a_prefabName, EntityHandle a_handleToEntityToCache);

	private:
		RenderSystem& m_renderingSystem;
		std::unordered_map<std::string, PrefabbedResource> m_resourceMap;
		ECSWrapper& m_entityDatabase;
	};

}