#include "pch.h"
#include "Graphics/DebugRenderer.h"
#if defined (PLATFORM_WINDOWS)
#include <glad/glad.h>
#elif defined (PLATFORM_SWITCH)

#endif
#include <spdlog/spdlog.h>
#include "Resources/Shader.h"
#include <memory>

namespace Frac
{
	//Line vs shader.
	const char* const vsSource =
			"#version 460 core												\n\
			layout (location = 1) in vec3 a_position;						\n\
			layout (location = 2) in vec4 a_color;							\n\
			layout (location = 1) uniform mat4 u_worldviewproj;				\n\
			out vec4 v_color;												\n\
																			\n\
			void main()														\n\
			{																\n\
				v_color = a_color;											\n\
				gl_Position = u_worldviewproj * vec4(a_position, 1.0);		\n\
			}";

	//Line fragment shader.
	const char* const fsSource =
			"#version 460 core												\n\
			in vec4 v_color;												\n\
			out vec4 frag_color;											\n\
																			\n\
			void main()														\n\
			{																\n\
				frag_color = v_color;										\n\
			}";

	void DebugRenderer::Initialize()
	{
		unsigned int vsShader = 0;
		unsigned int fsShader = 0;
		bool res = false;

		m_linesShaderProgram = glCreateProgram();

		res = Shader::CompileShader(&vsShader, GL_VERTEX_SHADER, vsSource);
		if (!res)
		{
			spdlog::error("DebugRenderer failed to compile vertex shader");
			return;
		}

		res = Shader::CompileShader(&fsShader, GL_FRAGMENT_SHADER, fsSource);
		if (!res)
		{
			spdlog::error("DebugRenderer failed to compile fragment shader");
			return;
		}

		glAttachShader(m_linesShaderProgram, vsShader);
		glAttachShader(m_linesShaderProgram, fsShader);

		if (!Shader::LinkProgram(m_linesShaderProgram))
		{
			glDeleteShader(fsShader);
			glDeleteShader(vsShader);
			glDeleteProgram(m_linesShaderProgram);
			spdlog::error("DebugRenderer failed to link shader program");
			return;
		}

		glDeleteShader(vsShader);
		glDeleteShader(fsShader);

		glCreateVertexArrays(1, &m_linesVAO);
		glBindVertexArray(m_linesVAO);

		glGenBuffers(1, &m_linesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);

		const auto size = sizeof(m_vertexArray);
		glBufferData(GL_ARRAY_BUFFER, size, &m_vertexArray[0], GL_STREAM_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosition3DColor), reinterpret_cast<void*>(offsetof(VertexPosition3DColor, Position)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPosition3DColor), reinterpret_cast<void*>(offsetof(VertexPosition3DColor, Color)));

#if _DEBUG
		glBindVertexArray(0);
#endif
	}

	void DebugRenderer::ShutDown()
	{
		glDeleteProgram(m_linesShaderProgram);
		glDeleteVertexArrays(1, &m_linesVAO);
		glDeleteBuffers(1, &m_linesVBO);
	}

	void DebugRenderer::AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color)
	{
		if(m_linesCount < m_maxLines)
		{
			m_vertexArray[m_linesCount * 2].Position = from;
			m_vertexArray[m_linesCount * 2].Color = color;
			m_vertexArray[m_linesCount * 2 + 1].Position = to;
			m_vertexArray[m_linesCount * 2 + 1].Color = color;
			++m_linesCount;
		}
	}

	void DebugRenderer::Render(const glm::mat4& viewProjectionMatrix)
	{
		glUseProgram(m_linesShaderProgram);
		glUniformMatrix4fv(1, 1, false, &viewProjectionMatrix[0][0]);
		glBindVertexArray(m_linesVAO);

		if (m_linesCount > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_linesVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexArray), &m_vertexArray[0], GL_DYNAMIC_DRAW);
			glDrawArrays(GL_LINES, 0, m_linesCount * 2);
#if _DEBUG
			glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
		}
	}

	void DebugRenderer::Reset()
	{
		m_linesCount = 0;
	}
}