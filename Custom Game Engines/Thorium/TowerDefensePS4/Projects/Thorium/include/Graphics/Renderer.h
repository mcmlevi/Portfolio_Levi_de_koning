#pragma once

#ifdef PLATFORM_WIN64
#include <Graphics/D3D12/D3D12Renderer.h>
#else
#include <Graphics/PS4/PS4Renderer.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12Renderer Renderer;
#else
	typedef PS4Renderer Renderer;
#endif
}