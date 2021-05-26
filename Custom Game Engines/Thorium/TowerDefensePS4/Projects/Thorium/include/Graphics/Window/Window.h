#pragma once

#ifdef PLATFORM_WIN64
#include <Graphics/Window/Win32Window.h>
#else
#include <Graphics/Window/PS4Window.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef Win32Window Window;
#else
	typedef PS4Window Window;
#endif
}