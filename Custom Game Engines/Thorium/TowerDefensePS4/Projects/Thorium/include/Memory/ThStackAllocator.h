#pragma once
#ifdef PLATFORM_WIN64
#include <Memory/StackAllocator.h>
#elif PLATFORM_PS4
#include "Memory/PS4StackAllocator.h"
#else
#error No Platform macro detected
#endif // PLATFORM_WIN64

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef StackAllocator StackAllocatorBehavior;
#elif PLATFORM_PS4
	typedef PS4StackAllocator StackAllocatorBehavior;
#endif // PLATFORM_WIN64

}