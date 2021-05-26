#pragma once

#include <glm/vec2.hpp>

namespace Frac
{
	/*!
	 * @brief TEMP CLASS FOR TEXT TEXTURES UNTIL RENDERCORE IS READY
	 */
	class TextTexture
	{
	public:
		TextTexture() = default;
		virtual ~TextTexture() = default;
		void LoadTexture(const std::string& filePath);
		void LoadTexture(void* data, glm::ivec2 size, int sizeOfElement, int channels);
		void Use();
		glm::ivec2 GetSize();
		int GetChannels();
		int GetID();
	protected:
		glm::ivec2 m_size;
		int m_channels;
		unsigned int m_textureID;
	};
}
