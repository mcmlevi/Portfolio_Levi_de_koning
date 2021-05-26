#pragma once

#ifdef PLATFORM_WIN64
#include <Graphics/D3D12/D3D12DebugRenderer.h>
#else
#include <Graphics/PS4/PS4DebugRenderer.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12DebugRenderer DebugRenderer;
#else
	typedef PS4DebugRenderer DebugRenderer;
#endif
}