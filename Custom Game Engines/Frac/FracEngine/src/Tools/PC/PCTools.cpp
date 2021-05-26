#include "pch.h"
#include "Tools\PC\PCTools.h"

#include "Core/ECSWrapper.h"

namespace Frac
{
	PCTools::PCTools()
	{
		m_graphicalUserInterface = std::make_unique<GraphicalUserInterface>();
	}

	void PCTools::Update()
	{
		m_graphicalUserInterface->Update();
	}
} // namespace Frac