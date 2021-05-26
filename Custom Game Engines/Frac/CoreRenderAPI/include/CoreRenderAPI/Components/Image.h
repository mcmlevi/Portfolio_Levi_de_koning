#pragma once

namespace TOR
{
	/// <summary>
	/// Used to identify the type of image data.
	/// </summary>
	enum class EImageChannelType
	{
		//Channels.
		RED		= 0x1903,
		GREEN	= 0x1904,
		BLUE	= 0x1905,
		ALPHA	= 0x1906,
		RGB		= 0x1907,
		RGBA	= 0x1908,

		//Floating point precision.
		RGBA32F = 0x8814,
		RGB32F	= 0x8815,
		RGBA16F = 0x881A,
		RGB16F	= 0x881B,

		//SRGB.
		SRGB					= 0x8C40,
		SRGB8					= 0x8C41,
		SRGB_ALPHA				= 0x8C42,
		SRGB8_ALPHA8			= 0x8C43,

		//Compressed channel types.
		COMPRESSED_SRGB			= 0x8C48,
		COMPRESSED_SRGB_ALPHA	= 0x8C49,
		COMPRESSED_RED			= 0x8225,
		COMPRESSED_RG			= 0x8226
	};

	/// <summary>
	/// Contains all specific image property data.
	/// </summary>
	struct Image
	{
		const char* FilePath;
		int Width;
		int Height;
		int Format;
		int BitSize;
	};
}