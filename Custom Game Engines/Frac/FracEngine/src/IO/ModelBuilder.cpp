#include "pch.h"
#include "Core/FileIO.h"
#include "IO/ModelBuilder.h"

namespace Frac
{
	ModelBuilder::ModelBuilder(Frac::RenderSystem& renderSystem, PrefabManager& prefabManager, ECSWrapper& resourceHandler, const std::string& filePath, ELoadType loadType) :
		m_prefabManager(prefabManager),
		m_renderSystem(renderSystem),
		m_entityManager(resourceHandler),
		m_builderType(loadType),
		m_ModelDirectory(""),
		m_ModelName("")
	{
		const std::size_t extentionCaretPos = filePath.find_last_of('.');
		const std::size_t extentionStringLength = filePath.size() - extentionCaretPos;
		m_ModelName = FileIO::GetNameFromFilePath(filePath);
		m_ModelDirectory = FileIO::GetPathFromWildcard("[Assets]") + filePath.substr(0, filePath.size() - m_ModelName.size() - extentionStringLength);
	}

	EntityHandle ModelBuilder::GetModel()
	{
		return m_finalModel;
	}

}