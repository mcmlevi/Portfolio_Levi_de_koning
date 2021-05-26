#pragma once
#include "Core/FileIO.h"

namespace Frac 
{
	/**
	* The ResourceManager's responsiblity is managing the lifetime of resources. 
	* It has templated functions related to managaing the resources such as loading new sources or getting existing resources.
	*/
	class ResourceManager 
	{
	public:
		ResourceManager();
		~ResourceManager() = default;

		FileIO& GetFileIO();

	private:
		std::unique_ptr<FileIO> m_fileIO;
	};

} // namespace Frac