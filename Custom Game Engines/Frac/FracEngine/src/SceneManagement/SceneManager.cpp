#include "pch.h"
#include "SceneManagement/SceneManager.h"
#include "IO/ModelDirector.h"
#include "IO/GLTFModelBuilder.h"
#include "Core/ECSWrapper.h"
#include "Resources/PrefabManager.h"

namespace Frac 
{
	SceneManager::SceneManager(RenderSystem& a_renderSystem, PrefabManager& prefabManager, ECSWrapper& a_ecsWrapper) :
        m_prefabManager(prefabManager),
        m_renderSystem(a_renderSystem), 
        m_ecsWrapper(a_ecsWrapper)
	{

	}

    EntityHandle SceneManager::GetModel(const std::string& a_filePath, EModelFileType type)
    {
        std::string fullFilePath = FileIO::GetPathFromWildcard(a_filePath);

        ModelDirector director;
        EntityHandle finalProduct;

        if (m_sceneCache.find(a_filePath) != m_sceneCache.end())
        {
			EntityHandle oldInstance = m_sceneCache[a_filePath].first;
			std::string nameOfModel = FileIO::GetNameFromFilePath(a_filePath);
			EntityHandle newPrefab = m_ecsWrapper.makeEntity(nameOfModel + "_SCENE_" + std::to_string(m_sceneCache[a_filePath].second));
			for (EntityHandle& handle : m_ecsWrapper.getChilderen(oldInstance))
			{
				std::stringstream ss(m_ecsWrapper.getType(handle));
				std::vector<std::string> result;

				while (ss.good())
				{
					std::string substr;
					std::getline(ss, substr, ',');
					result.push_back(substr);
				}

				for (std::string subString : result)
				{
					if (subString.find("INSTANCEOF|") != std::string::npos) {
						std::string TestString = subString.substr(11, subString.size() - 11);
						m_ecsWrapper.addChild(m_prefabManager.GetPrefabInstance(subString.substr(11,subString.size() - 11)), newPrefab);
					}
				}
			}
			m_sceneCache[nameOfModel].second++;
			return newPrefab;
        }
        else{
			switch (type)
			{
			case EModelFileType::GLTF:
			{
				GLTF_ModelBuilder builder(m_renderSystem, m_prefabManager, m_ecsWrapper, a_filePath, ELoadType::MODEL, false);
				director.SetBuilder(&builder);
				director.Construct();
				finalProduct = builder.GetModel();
				break;
			}
			case EModelFileType::GLB:
			{
				GLTF_ModelBuilder builder(m_renderSystem, m_prefabManager, m_ecsWrapper, a_filePath, ELoadType::MODEL, true);
				director.SetBuilder(&builder);
				director.Construct();
				finalProduct = builder.GetModel();
				break;
			}
			}
			m_sceneCache.insert(std::make_pair(a_filePath,std::make_pair(finalProduct,1)));
        }

        return finalProduct;
    }

    void SceneManager::SetEnvironmentModel(const std::string& a_filePath, const std::string& a_sceneEnvironmentName)
    {

    }

}