#pragma once
#ifdef PLATFORM_WIN64
#include <Graphics/ModelLoading/GLTFFactoryWin.h>

#elif PLATFORM_PS4
#include <Graphics/ModelLoading/GLTFFactoryPS4.h>
#endif

namespace Th
{
#ifdef PLATFORM_WIN64
	typedef GLTFFactoryWin GLTFFactory;
#elif PLATFORM_PS4
	typedef GLTFFactoryPS4 GLTFFactory;
#endif

}
