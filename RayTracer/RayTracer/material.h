#pragma once

#include "STB/stb_image.h"
#include "image.h"

struct Material
{
	Material(){}
	Material(float reflectI, float Albedo, float refractionI , Image* Texture = nullptr, Image* BumpMap = nullptr) :reflectiveIndex(reflectI), albedo(Albedo), refractionIndex(refractionI), texture(Texture),bumpMap(BumpMap)
	{	}
	~Material()
	{
		texture = nullptr;
		bumpMap = nullptr;
	}
	float reflectiveIndex{ 0.f };
	float albedo{ 0.18f };
	float refractionIndex{ 0.f };
	Image* texture{nullptr};
	Image* bumpMap{nullptr};
};
