#pragma once
#include "Graphics/TextRenderSystem/TextMesh.h"

#include <string>
#include <memory>

namespace Frac
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec2 Texture;
	};
	
	struct TextComponent
	{
		enum class Alignment
		{
			Left = 0,
			Center,
			Right
		};

		enum class FontFamily
		{
			Regular = 0,
			Icons
		};

		std::string	text;
		float fontSize			= 0.05f;
		glm::vec3 color			= glm::vec3(1.f, 1.f, 1.f);
		FontFamily fontFamily	= FontFamily::Regular;
		bool faceActiveCamera   = false;
		float angle				= 0.f;
		Alignment alignment		= Alignment::Center;
		float customKerning		= 0.0f;
		glm::vec3 offset		= glm::vec3(0.f);
		
		Frac::TextMesh mesh;
		
		std::string	dirtyText;

		bool isActive = true;
	};
}
