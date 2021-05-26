#pragma once

#include <cstddef>			// size_t
#include <memory>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <cstdint>			//uint_64

// IO
#include <sstream>
#include <ostream>
#include <iostream>
#include <cstdio>

// Containers
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <queue>
#include <stack>
#include <list>

#if defined(PLATFORM_WINDOWS)

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#ifdef APIENTRY
#undef APIENTRY
#endif

#include <windows.h>
#endif
#elif defined(PLATFORM_SWITCH)



#endif
