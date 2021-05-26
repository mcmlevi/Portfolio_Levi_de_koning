#pragma once
#include <CoreRenderAPI/Components/Texture.h>
#include <CoreRenderAPI/Components/Image.h>
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLSampler.h"

namespace TOR
{
	/// <summary>
	/// The texture class contains all the pointers required to access the texture data on the GPU.
	/// </summary>
	class OpenGLTexture
	{
	public:
		OpenGLTexture() = default;
		~OpenGLTexture() = default;

		void Initialize(const Image& image, const OpenGLSampler& sampler, unsigned char* source, int sourceCountInElements, int TextureFormat = GL_RGBA);
		void Destroy();

		GLuint GetId() const;
		GLuint GetSlot() const;

		void Bind(GLuint slot);
		void Unbind();

	private:
		Image m_Image;
		GLuint m_id;
		GLuint m_slot;
	};
}