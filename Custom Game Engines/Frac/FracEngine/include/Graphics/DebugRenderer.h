#pragma once
#include <glm\vec3.hpp>
#include <glm\vec4.hpp>
#include <glm\mat4x4.hpp>

namespace Frac
{
	/// <summary>
	/// Contains immediate drawing debug utility methods.
	/// </summary>
	class DebugRenderer
	{
	public:
		DebugRenderer() = default;
		~DebugRenderer() = default;

		DebugRenderer(const DebugRenderer& debugRenderer) = delete;
		void operator=(const DebugRenderer& debugRenderer) = delete;

		void Initialize();
		void ShutDown();

		void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec4& color);
		void Render(const glm::mat4& viewProjectionMatrix);
		void Reset();

	private:
		static int const m_maxLines = 32768;
		int m_linesCount;

		struct VertexPosition3DColor
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		VertexPosition3DColor m_vertexArray[m_maxLines * 2];
		unsigned int m_linesShaderProgram;
		unsigned int m_linesVAO;
		unsigned int m_linesVBO;
	};
}