#pragma once
#ifdef PLATFORM_WIN64
#include <Graphics/ModelLoading/ObjFactoryWin.h>

#elif PLATFORM_PS4
#include <Graphics/ModelLoading/ObjFactoryPS4.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef ObjFactoryWin ObjFactory;
#elif PLATFORM_PS4
	typedef ObjFactoryPS4 ObjFactory;
#endif
	
}
