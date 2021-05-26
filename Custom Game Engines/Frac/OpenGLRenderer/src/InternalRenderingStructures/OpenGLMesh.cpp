#include "OpenglRenderer/pch.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLMesh.h"

namespace TOR
{
	OpenGLMesh::OpenGLMesh() :
		m_VAO(0),
		m_EBO(0)
	{
	}

	void OpenGLMesh::BufferAttributeData(EVertexAttributes& attrType, uint32_t byteLength, uint32_t ByteStride, int attrElementCount, const void* bufferStart, unsigned long DataType /*= GL_FLOAT*/, unsigned long DrawMode /*= GL_STATIC_DRAW*/)
	{
		glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_VBO[(int)attrType]);
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO[(int)attrType]);
		glBufferData(GL_ARRAY_BUFFER, byteLength, bufferStart, DrawMode);
		glEnableVertexAttribArray((int)attrType);

		glVertexAttribPointer((int)attrType, attrElementCount, DataType, GL_FALSE, ByteStride, nullptr);
	}

	void OpenGLMesh::BufferElementData(uint32_t byteLength, void* bufferStart, unsigned long DrawMode /*= GL_DYNAMIC_DRAW*/)
	{
		// Index buffer
		glGenBuffers(1, &m_EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteLength, bufferStart, DrawMode);
	}

	void OpenGLMesh::Initialize()
	{
		glGenVertexArrays(1, &m_VAO);
	}

	void OpenGLMesh::Destroy()
	{
		glDeleteVertexArrays(1, &m_VAO);
	}

	void OpenGLMesh::Bind() const
	{
		glBindVertexArray(m_VAO);
	}

	void OpenGLMesh::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLMesh::SetShader(std::shared_ptr<OpenGLShader> shader)
	{
		m_shader = shader;
	}

	std::shared_ptr<OpenGLShader> OpenGLMesh::GetShader()
	{
		return m_shader;
	}

	void OpenGLMesh::Draw(unsigned indexCount)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, nullptr);
	}
}