#pragma once
#include <Graphics/ModelLoading/IModelFactory.h>

namespace Th
{
	class ObjFactoryWin : public IModelFactory
	{
	public:
		ObjFactoryWin() = default;
		virtual  ~ObjFactoryWin() override;
		Th::ModelData* LoadModel(const std::string& filePath) override;
		
	};
}


