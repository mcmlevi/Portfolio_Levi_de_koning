#include "CoreRenderAPI/pch.h"
#include "CoreRenderAPI/LowLevelAPI.h"
#include "CoreRenderAPI/DLLLoader.h"
#include "CoreRenderAPI/Components/Mesh.h"

namespace TOR
{
	LowLevelAPI* m_lowLevelRenderingCore = nullptr;

	typedef LowLevelAPI* (*createCoreFunction)();
	static createCoreFunction createCore;


	LowLevelAPI* LowLevelAPI::m_lowLevelRenderer = nullptr;

	LowLevelAPI* LowLevelAPI::CreateCoreAPI(const char* dllName)
	{
		if (!m_lowLevelRenderer)
		{
			DLLModule module = LoadTORModule(dllName);
			createCore = (createCoreFunction)GetSymbol(module, "CreateCore");
			if (createCore == nullptr) 
			{
				std::cout << ("Could not find CreateCore in library") << std::endl;
			}
			else
			{
				m_lowLevelRenderer = createCore();
			}
		}
		return m_lowLevelRenderer;
	}
}
