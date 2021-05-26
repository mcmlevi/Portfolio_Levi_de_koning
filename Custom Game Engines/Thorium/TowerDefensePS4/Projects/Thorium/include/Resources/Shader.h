#pragma once

#ifdef PLATFORM_WIN64
#include <Resources/D3D12/D3D12Shader.h>
#else
#include <Resources/PS4/PS4Shader.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12Shader Shader;
#else
	typedef PS4Shader Shader;
#endif
}