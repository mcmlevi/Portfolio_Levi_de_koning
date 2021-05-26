#pragma once
#include <Graphics/ModelLoading/IModelFactory.h>
namespace Th 
{
	class GLTFFactoryWin : public IModelFactory
	{
	public:
		// Inherited via IModelFactory
		virtual Th::ModelData* LoadModel(const std::string& filePath) final;
	private:
		static std::string GetFilePathExtension(const std::string& FileName);
	};

}

