#include "pch.h"
#include "IO/ModelDirector.h"
#include "Logger.h"

namespace Frac
{
	void ModelDirector::SetBuilder(ModelBuilder* modelBuilder)
	{
		m_Builder = modelBuilder;
	}

	void ModelDirector::Construct()
	{
		if(!m_Builder->LoadModelFromAPI())
		{
			LOGERROR("ERROR: Failed to load in the model from the defined API.");
		}

		if(!m_Builder->ParseModelData())
		{
			LOGERROR("ERROR: Failed to convert the model to the desired model format.");
		}
	}

}