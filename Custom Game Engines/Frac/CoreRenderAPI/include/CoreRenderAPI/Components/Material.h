#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "CoreRenderAPI/Components/Texture.h"

namespace TOR 
{
	//NOTE:
	//The occlusion map can (and should be) merge dwith the metalnessRoughness texture.
	//The 3 channel model is defined as: R = AO, G = Roughness, B = metallic. - blender also exports it this way, 
	//through the use of an in-editor export hack, because of prinicpled BRDF limitations: https://docs.blender.org/manual/en/latest/addons/import_export/scene_gltf2.html

	/// <summary>
	/// The alpha value regarding the transparency in the material.
	/// </summary>
	enum class EAlphaMode
	{
		OPAQUE_MODE = 0,
		MASK_MODE,
		BLEND_MODE,

		COUNT
	};

	/// <summary>
	/// The material class follows a PBR metallic Roughness material approach.
	/// </summary>
	struct Material
	{
		glm::vec4 AlbedoFactor;
		glm::vec3 EmissionFactor;
		float MetallicFactor;
		float RoughnessFactor;

		EAlphaMode AlphaMode;
		float AlphaCutoff = 0.5f;

		float NormalScale;
		bool FlipNormals;

		bool HasOcclusionMap;
		bool DoubleSided;

		int TexCoords[static_cast<int>(ETextureType::COUNT)];
	};

	/// <summary>
	/// The PBR material supports only the specific PBR maps:
	/// - Albedo.
	/// - Normal.
	/// - MetalRougnessAO.
	/// - Emission.
	/// </summary>
	struct PBRMaterial
	{
		Material& Properties;
		const TOR::AlbedoTexture* albedoMap;
		const TOR::NormalTexture* normalMap;
		const TOR::MetalRougnessAOTexture* mraoMap;
		const TOR::EmissionTexture* emissionMap;
	};
}