#pragma once
#include <CoreRenderAPI/Components/Mesh.h>
#include <CoreRenderAPI/Components/Material.h>
#include <OpenGLRenderer/InternalRenderingStructures/OpenGLShader.h>

namespace TOR
{
	class OpenGLMesh 
	{
	public:	
		OpenGLMesh();
		~OpenGLMesh() = default;

		void Initialize();
		void Destroy();

		void BufferAttributeData(EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, unsigned long DataType = GL_FLOAT, unsigned long DrawMode = GL_STATIC_DRAW);
		void BufferElementData(uint32_t byteLength, void* bufferStart, unsigned long DrawMode = GL_DYNAMIC_DRAW);

		void Bind() const;
		void Unbind() const;

		void SetShader(std::shared_ptr<OpenGLShader> shader);
		std::shared_ptr<OpenGLShader> GetShader();

		void Draw(unsigned indexCount);
	private:
		std::shared_ptr<OpenGLShader> m_shader;

		GLuint m_VAO;
		std::array<GLuint, 8> m_VBO;
		GLuint m_EBO;
	};
}