#pragma once
#include <string>
#include "CoreRenderAPI/Components/Image.h"

namespace TOR
{
	/// <summary>
	/// The different supported texture map types.
	/// </summary>
	enum class ETextureType
	{
		ALBEDO = 0,
		METALROUGHNESS,
		NORMAL,
		AO,
		EMISSION,

		COUNT
	};
	/// <summary>
	/// The texture class contains all the pointers required to access the texture data on the GPU.
	/// </summary>
	struct Texture
	{
		Image TexImage;
		//ETextureType TextureType;
		unsigned Id;
	};

	struct AlbedoTexture { Texture tex; };
	struct MetalRougnessAOTexture { Texture tex; };
	struct NormalTexture { Texture tex; };
	struct EmissionTexture { Texture tex; };
}