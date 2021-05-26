#include <Thpch.h>
#include <Utility/D3DErrorHandler.h>
#include <Core/MessageLogger.h>
namespace Th
{
	D3DErrorHandler::D3DErrorHandler()
	{
		D3D_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugInterface)));
		m_DebugInterface->EnableDebugLayer();
	}

	void D3DErrorHandler::CheckD3DError(long hr, const char* method, const std::string& file, int lineNumber)
	{
		if (FAILED(hr))
		{
			LOGERROR("D3D12 error in %s on line number %i in '%s'.\n", file.c_str(), lineNumber, method);
			__debugbreak();
		}
	}
}