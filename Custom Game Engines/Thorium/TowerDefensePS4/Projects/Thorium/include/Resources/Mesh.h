#pragma once

#ifdef PLATFORM_WIN64
#include <Resources/D3D12/D3D12Mesh.h>
#elif PLATFORM_PS4
#include <Resources/PS4/PS4Mesh.h>
#endif


namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12Mesh Mesh;
#elif defined PLATFORM_PS4
	typedef PS4Mesh Mesh;
#endif
}