#pragma once
namespace TOR
{

#if defined(PLATFORM_WINDOWS)


	static HMODULE module;
	typedef HMODULE DLLModule;
	bool GetLastErrorString( char* buffer, DWORD length);
	FARPROC GetSymbol(DLLModule module, const char* symbol);
#elif defined(PLATFORM_SWITCH)			

#endif
	std::string GetDLLRootPath();
	const DLLModule& LoadTORModule(const char* dllName);
	void UnloadModule();
	size_t ReadAll(void* pOut, size_t bufferSize, const char* path);

}