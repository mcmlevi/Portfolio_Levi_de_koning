#pragma once

#ifdef PLATFORM_WIN64
#include <Resources/D3D12/D3D12Vertex.h>
#elif PLATFORM_PS4
#include <Resources/PS4/PS4Vertex.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12Vertex Vertex;
#elif defined PLATFORM_PS4
	typedef PS4Vertex Vertex;
#endif
}