#pragma once
#include "Graphics/RenderSystem.h"
#include "Core/ECSWrapper.h"
#include "IO/ModelBuilder.h"

namespace Frac
{
	class SceneManager
	{
	public:
		SceneManager(RenderSystem& a_renderSystem, PrefabManager& prefabManager, ECSWrapper& a_ecsWrapper);
		~SceneManager() = default;

		EntityHandle GetModel(const std::string& a_filePath, EModelFileType type);
		void SetEnvironmentModel(const std::string& a_filePath, const std::string& a_sceneEnvironmentName);

	private:
		PrefabManager& m_prefabManager;
		RenderSystem& m_renderSystem;
		ECSWrapper& m_ecsWrapper;

		std::unordered_map<std::string,std::pair<EntityHandle, int>> m_sceneCache;
	};
}