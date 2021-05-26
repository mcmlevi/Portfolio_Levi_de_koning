#pragma once
#include <CoreRenderAPI/Components/Sampler.h>

//#TODO: cache the gl samplers, since these are a lot of times the same.

namespace TOR
{
	/// <summary>
	/// The OpenGL Sampler holds the texture sampling properties.
	/// </summary>
	struct OpenGLSampler
	{
		int MinFilter;
		int MagFilter;
		int WrapFilterS;
		int WrapFilterT;
		int WrapFilterR;
	};

	extern int ParseSampleFilterMethodToOpenGL(ESampleFilterMethod method);
	extern int ParseTextureWrapMethodToOpenGL(ETextureWrapMethod method);
	extern OpenGLSampler ParseSamplerToGLSampler(const Sampler& sampler);
}