#pragma once

#ifdef PLATFORM_WIN64
#include <Graphics/D3D12/D3D12RenderContext.h>
#elif defined (PLATFORM_PS4)
#include <Graphics/PS4/PS4RenderContext.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12RenderContext RenderContext;
#elif defined PLATFORM_PS4
	typedef PS4RenderContext RenderContext;
#endif
}