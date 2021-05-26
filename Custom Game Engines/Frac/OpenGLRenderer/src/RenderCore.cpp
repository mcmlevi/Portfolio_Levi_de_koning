#include "OpenGLRenderer/pch.h"
#include "CoreRenderAPI/LowLevelAPI.h"
#include "OpenGLRenderer/OpenGL_Renderer.h"

extern "C" TOR_API TOR::LowLevelAPI* CreateCore()
{
	return new TOR::OpenGL_Renderer();
}