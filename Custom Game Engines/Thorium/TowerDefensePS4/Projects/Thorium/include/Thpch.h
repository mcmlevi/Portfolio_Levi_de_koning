#pragma once

// Thpch.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

// TODO: reference additional headers your program requires here

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

//Memory management
#include <Memory/ThLinearAllocator.h>
#include <Memory/ThPoolAllocator.h>
#include <Memory/ThFreeListAllocator.h>
#include <Memory/ThStackAllocator.h>
#include <Memory/MemoryManager.h>


//Mathematics
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

//Platform specific.
#if PLATFORM_WIN64

//Windows API.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>			//The WinAPI
#include <wrl.h>				//Comptr
#include <shellapi.h>			//Command line args

using namespace Microsoft::WRL; //Only used in the precomp headers, never use this in a header file.

//D3D12 defines that shouldn't exist.
#pragma region Windows specific undefines
#if defined max
#undef max
#endif

#if defined min
#undef min
#endif

#if defined CreateWindow
#undef CreateWindow
#endif
#pragma endregion

//D3D12 includes and deps.
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <Utility/d3dx12.h>

//D3D12 Static lib includes.
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")

#elif PLATFORM_PS4



#else

#error No Platform macro detected

#endif