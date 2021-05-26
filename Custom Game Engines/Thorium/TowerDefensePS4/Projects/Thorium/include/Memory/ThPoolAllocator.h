#pragma once
#ifdef PLATFORM_WIN64
#include <Memory/PoolAllocator.h>
#elif PLATFORM_PS4
#include "Memory/PS4PoolAllocator.h"
#else
#error No Platform macro detected
#endif // PLATFORM_WIN64



namespace Th
{
#ifdef PLATFORM_WIN64
	typedef PoolAllocator PoolAllocatorBehavior;
#elif PLATFORM_PS4
	typedef PS4PoolAllocator PoolAllocatorBehavior;
#endif // PLATFORM_WIN64

}