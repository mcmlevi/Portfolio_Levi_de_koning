#pragma once
#include <CoreRenderAPI/Components/Model.h>
#include "Core/ECSWrapper.h"
#include "Graphics/RenderSystem.h"
#include "Resources/PrefabManager.h"

namespace Frac
{
	/// <summary>
	/// Used to identify which file type should be loaded.
	/// </summary>
	enum class EModelFileType
	{
		GLTF = 0,
		GLB
	};

	enum class ELoadType
	{
		ENVIRONMENT,
		LIGHT,
		MODEL
	};

	/// <summary>
	/// Interface for the model loading.
	/// </summary>
	class ModelBuilder
	{
	public:
		ModelBuilder(Frac::RenderSystem& renderSystem, PrefabManager& prefabManager, ECSWrapper& resourceHandler, const std::string& filePath, ELoadType loadType);
		virtual ~ModelBuilder() = default;
		
		virtual bool LoadModelFromAPI() = 0;
		virtual bool ParseModelData() = 0;
		EntityHandle GetModel();
	protected:
		PrefabManager& m_prefabManager;
		ECSWrapper& m_entityManager;
		RenderSystem& m_renderSystem;
		ELoadType m_builderType;
		EntityHandle m_finalModel;
		std::string m_ModelName, m_ModelDirectory;
	};
}