#pragma once
namespace TOR 
{
	enum class EShaderType
	{
		Diffuse,
		DiffusePBR,
		DiffuseWithNormalMapping,
		ShadowMapping
	};

	enum class ETorDataType
	{
		BYTE = 0x1400,
		UBYTE = 0x1401,
		SHORT = 0x1402,
		USHORT = 0x1403,
		INT = 0x1404,
		UINT = 0x1405,
		FLOAT = 0x1406,
		BYTES_2 = 0x1407,
		BYTES_3 = 0x1408,
		BYTES_4 = 0x1409,
		DOUBLE = 0x140A
	};

	enum class EVertexAttributes
	{
		POSITION = 0,
		NORMAL,
		TANGENT,
		TEXCOORD_0,
		TEXCOORD_1,
		COLOR_0,
		JOINTS_0,
		WEIGHTS_0
	};

	struct Mesh
	{
		unsigned Id;

		unsigned VertexCount;
		unsigned IndiceCount;

		ETorDataType ElementDataType;
	};
}