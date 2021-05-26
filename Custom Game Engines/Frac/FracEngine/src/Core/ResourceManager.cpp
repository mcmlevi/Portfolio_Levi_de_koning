#include "pch.h"
#include "Core\ResourceManager.h"

namespace Frac 
{
	ResourceManager::ResourceManager()
	{
		m_fileIO = std::make_unique<FileIO>();
	}

	FileIO& ResourceManager::GetFileIO()
	{
		return *m_fileIO;
	}
} // namespace Frac
