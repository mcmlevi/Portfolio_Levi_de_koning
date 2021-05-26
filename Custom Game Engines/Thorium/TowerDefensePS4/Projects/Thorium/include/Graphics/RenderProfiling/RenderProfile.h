#pragma once

#pragma once

#ifdef PLATFORM_WIN64
#include <Graphics/RenderProfiling/D3D12RenderProfile.h>
#else
#include <Graphics/RenderProfiling/PS4RenderProfile.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef D3D12RenderProfile RenderProfile;
#else
	typedef PS4RenderProfile RenderProfile;
#endif
}