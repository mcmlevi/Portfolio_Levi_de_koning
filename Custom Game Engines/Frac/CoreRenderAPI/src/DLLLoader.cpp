#include "CoreRenderAPI/pch.h"

#include "CoreRenderAPI/DLLLoader.h"

#include <cstdlib>







#if defined(PLATFORM_SWITCH)

#elif defined(PLATFORM_WINDOWS)
#ifdef TOR_API_EXPORTS
#define TOR_API __declspec(dllexport)
#else
#define TOR_API __declspec(dllimport)
#endif
#endif

#if defined(PLATFORM_WINDOWS)
	bool TOR::GetLastErrorString(char* buffer, DWORD length)
	{
		auto err = GetLastError();

		if (!err)
			return false;

		// Assuming buffer is large enough for any error message.
		// Otherwise, use FORMAT_MESSAGE_ALLOCATE_BUFFER and LocalFree!
		auto ret = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
			buffer,
			length,
			NULL);

		return ret > 0;
	}

	

FARPROC TOR::GetSymbol(DLLModule module, const char* symbol)
{
	return GetProcAddress(module, symbol);
}

#endif

#if defined (PLATFORM_SWITCH)


#endif
size_t TOR::ReadAll(void* pOut, size_t bufferSize, const char* path)
{
	#if defined(PLATFORM_SWITCH)
	;
	#elif defined(PLATFORM_WINDOWS)
	return 0;
	#endif
	return 0;
	
}

std::string TOR::GetDLLRootPath()
{
	// Path is hardcoded as it doesn't have access to the engine's wildcard system
#if defined (CONFIG_DEBUG) // Debug
#if defined (PLATFORM_WINDOWS) // x64
	return "dll/OpenGLRenderer/";
#elif defined (PLATFORM_SWITCH) // NX64
	
#endif

#elif defined (CONFIG_DEVELOP) // Develop
#if defined (PLATFORM_WINDOWS) // x64
	return "dll/OpenGLRenderer/";
#elif defined (PLATFORM_SWITCH) // NX64
	
#endif

#else // Release
#if defined (PLATFORM_WINDOWS) // x64
	return "dll/OpenGLRenderer/";
#elif defined (PLATFORM_SWITCH) // NX64
	
#endif
#endif
}

const TOR::DLLModule& TOR::LoadTORModule(const char* dllName)
{
	#if defined(PLATFORM_SWITCH)
	
	#elif defined(PLATFORM_WINDOWS)
	// try the development folder structure first
	std::string dllpath = GetDLLRootPath();
	
	dllpath += dllName;
	module = LoadLibraryA(dllpath.c_str());
	if (!module)
	{
		char errstr[1024] = "FormatMessage failed (unknown error code?)";
		// Print informative info:
		if (GetLastErrorString(errstr, sizeof(errstr)))
			printf("Failed to load %s: %s\tTrying local DLL\n", dllpath.c_str(), errstr);

		// see if the dll is perhaps in the current folder
		module = LoadLibraryA(dllName);
		if (!module)
		{
			GetLastErrorString(errstr, sizeof(errstr));
			printf("Unable to open %s: %s", dllName, errstr);
		}
	}
	return module;

	#endif
}

void TOR::UnloadModule()
{
	#if defined(PLATFORM_SWITCH)

	#elif defined(PLATFORM_WINDOWS)
	
	#endif
}
