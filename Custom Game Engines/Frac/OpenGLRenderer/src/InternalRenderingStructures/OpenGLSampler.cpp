#include "OpenGLRenderer/pch.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLSampler.h"

namespace TOR
{
	int ParseSampleFilterMethodToOpenGL(ESampleFilterMethod method)
	{
		switch (method)
		{
		case ESampleFilterMethod::NEAREST:
			return GL_NEAREST;
		case ESampleFilterMethod::LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case ESampleFilterMethod::LINEAR:
			return GL_LINEAR;
		case ESampleFilterMethod::LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		case ESampleFilterMethod::NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case ESampleFilterMethod::NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		default:
			printf("WARNING: Undefined SampleFilterMethod!\n");
		}

		return 0;
	}

	int ParseTextureWrapMethodToOpenGL(ETextureWrapMethod method)
	{
		switch(method)
		{
		case ETextureWrapMethod::REPEAT:
			return GL_REPEAT;
		case ETextureWrapMethod::MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case ETextureWrapMethod::CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		default:
			printf("WARNING: Undefined TextureWrapMethod!\n");
		}

		return 0;
	}

	OpenGLSampler ParseSamplerToGLSampler(const Sampler& sampler)
	{
		OpenGLSampler glSampler =
		{
			ParseSampleFilterMethodToOpenGL(sampler.MinFilter),
			ParseSampleFilterMethodToOpenGL(sampler.MagFilter),
			ParseTextureWrapMethodToOpenGL(sampler.WrapFilterS),
			ParseTextureWrapMethodToOpenGL(sampler.WrapFilterT),
			ParseTextureWrapMethodToOpenGL(sampler.WrapFilterR)
		};

		return glSampler;
	}
}