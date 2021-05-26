#include "OpenGLRenderer/pch.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLTexture.h"


namespace TOR
{
	void OpenGLTexture::Initialize(const Image& image, const OpenGLSampler& sampler, unsigned char* source, int sourceCountInElements, int TextureFormat)
	{
		m_Image = image;

		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_2D, m_id);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		GLenum format;
		switch (image.Format)
		{
		case 1:
			format = GL_RED;
			break;
		case 2:
			format = GL_RG;
			break;
		case 3:
			format = GL_RGB;
			break;
		default:
			format = GL_RGBA;
			break;
		}

		GLenum type;
		switch (image.BitSize)
		{
		case 8:
			type = GL_UNSIGNED_BYTE;
			break;
		case 16:
			type = GL_UNSIGNED_SHORT;
			break;
		default:
			type = GL_UNSIGNED_INT;
			break;
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.MinFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.MagFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.WrapFilterS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.WrapFilterT);

		glTexImage2D(GL_TEXTURE_2D, 0, (TextureFormat != GL_SRGB8_ALPHA8 ? format : TextureFormat), image.Width, image.Height, 0, format, type, &source[0]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLTexture::Destroy()
	{
		glDeleteTextures(1, &m_id);
	}

	GLuint OpenGLTexture::GetId() const
	{
		return m_id;
	}

	GLuint OpenGLTexture::GetSlot() const
	{
		return m_slot;
	}

	void OpenGLTexture::Bind(GLuint slot)
	{
		m_slot = slot;
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

	void OpenGLTexture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glActiveTexture(GL_TEXTURE0);
		m_slot = 0;
	}

}