#include "pch.h"
#include "Resources/PrefabManager.h"
#include "Graphics/RenderSystem.h"
#include "Core/ECSWrapper.h"
#include <CoreRenderAPI/RenderAPI.h>
#include <CoreRenderAPI/Components/Mesh.h>
#include <CoreRenderAPI/Components/Texture.h>
#include <CoreRenderAPI/Components/Material.h>

namespace Frac
{
	PrefabManager::PrefabManager(RenderSystem& a_renderingSystem, ECSWrapper& a_entityManager) : m_renderingSystem(a_renderingSystem), m_entityDatabase(a_entityManager)
	{
		m_resourceMap.reserve(5000);

		std::function<void(EntityHandle, SubResourceContainer&)> prefabSystem = [this](EntityHandle handle, SubResourceContainer& subResourceVector)
		{
			PrefabbedResource& resourceToUnreference = *m_entityDatabase.get<PrefabbedResource>(handle);
			if (--resourceToUnreference.refCount == 0)
			{	
				if(m_entityDatabase.has<SubResourceContainer>(handle))
				{
					const SubResourceContainer& subResourceContainerInEntity = subResourceVector;
					for (auto prefabSubResourceName : subResourceContainerInEntity.prefabResources)
					{
						PrefabbedResource& subResourceToUnreference = m_resourceMap[prefabSubResourceName.first];
						if (--resourceToUnreference.refCount == 0) // Handle all sub resource cases.
						{
							if (prefabSubResourceName.first.find("MESH") != std::string::npos)
							{
								const TOR::Mesh& meshComponent = *m_entityDatabase.get<TOR::Mesh>(subResourceToUnreference.entityWithResources);
								m_renderingSystem.GetRenderAPI().DestroyMesh(meshComponent.Id);
							}
							else if (prefabSubResourceName.first.find("MATERIAL") != std::string::npos)
							{
								
							}
							else if (prefabSubResourceName.first.find("IMAGE") != std::string::npos)
							{
								
							}
							else if (prefabSubResourceName.first.find("SAMPLER") != std::string::npos)
							{
								
							}
							else if (prefabSubResourceName.first.find("TEXTURE") != std::string::npos)
							{
								const TOR::Texture& textureComponent = *m_entityDatabase.get<TOR::Texture>(subResourceToUnreference.entityWithResources);
								m_renderingSystem.GetRenderAPI().DestroyTexture(textureComponent.Id);
							}
						}
					}
				}
			}
			m_resourceMap.erase(resourceToUnreference.resourceName);
		};
		a_entityManager.makeSystem<Frac::SubResourceContainer>(prefabSystem, Frac::ePipelineStage::OnRemove, "Unreference prefab system when something is deleted");
	
	}

	bool PrefabManager::HasPrefab(std::string a_prefabName)
	{
		return m_resourceMap.find(a_prefabName) != m_resourceMap.end();
	}

	EntityHandle PrefabManager::GetPrefab(std::string a_prefabName)
	{
		FRAC_ASSERT(m_resourceMap.find(a_prefabName) != m_resourceMap.end());
		
		m_resourceMap[a_prefabName].refCount++;
		return m_resourceMap[a_prefabName].entityWithResources;
	}

	EntityHandle PrefabManager::GetPrefabInstance(std::string a_prefabName)
	{
		FRAC_ASSERT(m_resourceMap.find(a_prefabName) != m_resourceMap.end());

		m_resourceMap[a_prefabName].refCount++;

		EntityHandle baseInstanceEntity = m_entityDatabase.makeFromPrefab(m_resourceMap[a_prefabName].entityWithResources, m_resourceMap[a_prefabName].resourceName + "_" + std::to_string(m_resourceMap[a_prefabName].refCount));
	

		if(m_entityDatabase.has<SubResourceContainer>(baseInstanceEntity)){
			SubResourceContainer& newInstanceSubResources = *m_entityDatabase.get<SubResourceContainer>(baseInstanceEntity);
			for (auto& i : newInstanceSubResources.prefabResources)
			{
				SubResourceContainer::TypeOfSubresource relativeType = i.second;

				switch (i.second) {
					case SubResourceContainer::TypeOfSubresource::CHILD:
					{
						m_entityDatabase.addChild(GetPrefabInstance(i.first),baseInstanceEntity);
						break;
					}
					case SubResourceContainer::TypeOfSubresource::ATTACHEMENT:
					{
						m_entityDatabase.AddInstancedPrefabToEntity(GetPrefabInstance(i.first), baseInstanceEntity);
						break;
					}
				}
			}
		}
		return baseInstanceEntity;
	}

	void PrefabManager::CachePrefab(std::string a_prefabName, EntityHandle a_handleToEntityToCache)
	{
		FRAC_ASSERT(m_resourceMap.find(a_prefabName) == m_resourceMap.end());
		PrefabbedResource newResource {0,a_prefabName,a_handleToEntityToCache};

		m_resourceMap.insert(std::make_pair(a_prefabName,newResource));
	}
}