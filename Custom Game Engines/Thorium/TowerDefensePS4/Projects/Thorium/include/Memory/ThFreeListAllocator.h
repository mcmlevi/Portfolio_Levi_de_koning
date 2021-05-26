#pragma once

#ifdef PLATFORM_WIN64
#include <Memory/FreeListAllocator.h>
#elif PLATFORM_PS4
#include "Memory/PS4FreeListAllocator.h"
#else
#error No Platform macro detected
#endif // PLATFORM_WIN64

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef FreeListAllocator FreeListAllocatorBehavior;
#elif PLATFORM_PS4
	typedef PS4FreeListAllocator FreeListAllocatorBehavior;
#endif // PLATFORM_WIN64

}