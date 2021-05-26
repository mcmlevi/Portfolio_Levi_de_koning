#pragma once

namespace TOR
{
	/// <summary>
	/// Used to identify how the sampler should handle the texture wrapping.
	/// </summary>
	enum class ETextureWrapMethod
	{
		REPEAT = 10497,
		MIRRORED_REPEAT = 33648,
		CLAMP_TO_EDGE = 33071,
	};

	/// <summary>
	/// The different sampler methods.
	/// </summary>
	enum class ESampleFilterMethod
	{
		NEAREST = 9728,
		LINEAR = 9729,
		NEAREST_MIPMAP_NEAREST = 9984,
		LINEAR_MIPMAP_NEAREST = 9985,
		NEAREST_MIPMAP_LINEAR= 9986,
		LINEAR_MIPMAP_LINEAR= 9987
	};


	


	/// <summary>
	/// Contains the properties for the wrapping of the texture.
	/// </summary>
	struct Sampler
	{
		ESampleFilterMethod MinFilter;
		ESampleFilterMethod MagFilter;
		ETextureWrapMethod WrapFilterS;
		ETextureWrapMethod WrapFilterT;
		ETextureWrapMethod WrapFilterR;

		inline static ETextureWrapMethod ConvertglTFWrapMethodToTORWrapMethod(unsigned long long value)
		{
			return static_cast<ETextureWrapMethod>(value);
		}

		inline static ESampleFilterMethod ConvertglTFFilterMethodToTORFilterMethod(unsigned long long value)
		{
			return static_cast<ESampleFilterMethod>(value);
		}
	};
}