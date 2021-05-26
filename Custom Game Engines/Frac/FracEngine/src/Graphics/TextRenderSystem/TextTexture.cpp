#include "pch.h"
#include "Graphics/TextRenderSystem/TextTexture.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>

#include "Graphics/OpenGL.h"

namespace Frac
{
	void TextTexture::LoadTexture(const std::string& filePath)
	{
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);

		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned char* data = stbi_load("container.jpg", &m_size.x, &m_size.y, &m_channels, 0);
		if(data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_size.x, m_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
		{
			LOGWARNING("[TextRenderSystem | Texture] Failed to load texture");
		}
		stbi_image_free(data);
	}

	void TextTexture::LoadTexture(void* data, glm::ivec2 size, int sizeOfElement, int channels)
	{
		m_size = size;
		m_channels = channels;
		glGenTextures(1, &m_textureID);
		glBindTexture(GL_TEXTURE_2D, m_textureID);


		glTexImage2D(GL_TEXTURE_2D, 0, channels, m_size.x, m_size.y, 0, channels, sizeOfElement, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}


	void TextTexture::Use()
	{
		glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		// set the texture wrapping/filtering options (on the currently bound texture object)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	}

	glm::ivec2 TextTexture::GetSize()
	{
		return m_size;
	}

	int TextTexture::GetChannels()
	{
		return m_channels;
	}

	int TextTexture::GetID()
	{
		return m_textureID;
	}
}


