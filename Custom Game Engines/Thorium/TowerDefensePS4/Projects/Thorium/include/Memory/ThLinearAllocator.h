#pragma once
#ifdef PLATFORM_WIN64
#include <Memory/LinearAllocator.h>
#elif defined PLATFORM_PS4
#include "Memory/PS4LinearAllocator.h"
#else
#error No Platform macro detected
#endif // PLATFORM_WIN64

namespace Th 
{
#ifdef PLATFORM_WIN64
	typedef LinearAllocator LinearAllocatorBehavior;
#elif defined PLATFORM_PS4
	typedef PS4LinearAllocator LinearAllocatorBehavior;
#endif // PLATFORM_WIN64
}