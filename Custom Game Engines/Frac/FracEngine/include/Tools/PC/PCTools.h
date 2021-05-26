#pragma once
#include "Tools/ITools.h"
#include "GraphicalUserInterface.h"

namespace Frac 
{
	class EngineCore;
	class PCTools : public ITools 
	{
	public:
		PCTools();
		~PCTools() = default;

		void Update() override; 

	private:
		std::unique_ptr<GraphicalUserInterface> m_graphicalUserInterface;
	};
} // namespace Frac