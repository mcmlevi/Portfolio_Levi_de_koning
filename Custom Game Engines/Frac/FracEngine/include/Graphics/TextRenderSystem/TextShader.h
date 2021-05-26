#pragma once

#include <string>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace Frac
{
	class TextShader
	{
	public:
		TextShader() = default;
		~TextShader();
		void LoadFromString(const std::string& vertexSource, const std::string& fragSource);
		void LoadFromFile(const std::string& vertexSourceFilePath, const std::string& fragSourceFilePath);
		void Use();
		void Unuse();
		void setBool(const std::string& name, bool value) const;

		void setInt(const std::string& name, int value) const;

		void setFloat(const std::string& name, float value) const;
		void setVec2(const std::string& name, glm::vec2& value) const;
		void setVec3(const std::string& name, glm::vec3& value) const;
		void setVec4(const std::string& name, glm::vec4& value) const;
		void setMat3(const std::string& name, glm::mat3& value) const;
		void setMat4(const std::string& name, glm::mat4& value) const;
	private:
		void CheckForError(unsigned int ID, unsigned int checkFor);
		void CompileShader(const std::string& source, unsigned int& shader, unsigned shaderType);
		void CreateProgram(unsigned int vertex, unsigned int frag);
		unsigned int m_program;
	};

}
