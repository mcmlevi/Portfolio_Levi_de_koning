#include "pch.h"

#include "Graphics/TextRenderSystem/TextShader.h"

#include <fstream>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/OpenGL.h"

namespace Frac
{
	TextShader::~TextShader()
	{
	    glDeleteProgram(m_program);
	}

	void TextShader::LoadFromString(const std::string& vertexSource, const std::string& fragSource)
	{
	    // 2. compile shaders
	    unsigned int vertex, fragment;
	    CompileShader(vertexSource, vertex,GL_VERTEX_SHADER);
		
	    CompileShader(fragSource, fragment, GL_FRAGMENT_SHADER);
		
	    CreateProgram(vertex, fragment);
	}

	void TextShader::LoadFromFile(const std::string& vertexSourceFilePath, const std::string& fragSourceFilePath)
	{
	    // 1. retrieve the vertex/fragment source code from filePath
	    std::string vertexCode;
	    std::string fragmentCode;
	    std::ifstream vTextShaderFile;
	    std::ifstream fTextShaderFile;
	    // ensure ifstream objects can throw exceptions:
	    vTextShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	    fTextShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	    try
	    {
	        // open files
	        vTextShaderFile.open(vertexSourceFilePath);
	        fTextShaderFile.open(fragSourceFilePath);
	        std::stringstream vTextShaderStream, fTextShaderStream;
	        // read file's buffer contents into streams
	        vTextShaderStream << vTextShaderFile.rdbuf();
	        fTextShaderStream << fTextShaderFile.rdbuf();
	        // close file handlers
	        vTextShaderFile.close();
	        fTextShaderFile.close();
	        // convert stream into string
	        vertexCode = vTextShaderStream.str();
	        fragmentCode = fTextShaderStream.str();

	        LoadFromString(vertexCode, fragmentCode);
	    }
	    catch (std::ifstream::failure& e)
	    {
			LOGERROR("[TextRenderSystem | TextShader] ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
	    }
	}

	void TextShader::Use()
	{
	    glUseProgram(m_program);
	}

	void TextShader::Unuse()
	{
		glUseProgram(0);
	}
	
	void TextShader::setBool(const std::string& name, bool value) const
	{
	    glUniform1i(glGetUniformLocation(m_program, name.c_str()), (int)value);
	}

	void TextShader::setInt(const std::string& name, int value) const
	{
	    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
	}

	void TextShader::setFloat(const std::string& name, float value) const
	{
	    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
	}

	void TextShader::setVec2(const std::string& name, glm::vec2& value) const
	{
	    glUniform2f(glGetUniformLocation(m_program, name.c_str()), value.x, value.y);
	}

	void TextShader::setVec3(const std::string& name, glm::vec3& value) const
	{
	    glUniform3f(glGetUniformLocation(m_program, name.c_str()), value.x,value.y,value.z);
	}

	void TextShader::setVec4(const std::string& name, glm::vec4& value) const
	{
	    glUniform4f(glGetUniformLocation(m_program, name.c_str()), value.x, value.y, value.z,value.w);
	}

	void TextShader::setMat3(const std::string& name, glm::mat3& value) const
	{
	    glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}

	void TextShader::setMat4(const std::string& name, glm::mat4& value) const
	{
	    glUniformMatrix4fv(glGetUniformLocation(m_program,name.c_str()),1,GL_FALSE,glm::value_ptr(value));
	}

	void TextShader::CheckForError(unsigned ID,GLenum checkFor)
	{
		int success;
		char infoLog[512];
		glGetShaderiv(ID, checkFor, &success);
		if (!success)
		{
			glGetShaderInfoLog(ID, 512, NULL, infoLog);
			LOGERROR("[TextRenderSystem | TextShader] ERROR::SHADER::VERTEX::COMPILATION_FAILED %s", infoLog);
		};
	}

	void TextShader::CompileShader(const std::string& source, unsigned& shader,  GLenum shaderType)
	{
		const char* vShaderCode = source.c_str();
		// vertex Shader
		shader = glCreateShader(shaderType);
		glShaderSource(shader, 1, &vShaderCode, NULL);
		glCompileShader(shader);
		// print compile errors if any
		CheckForError(shader,GL_COMPILE_STATUS);
	}
	void TextShader::CreateProgram(unsigned vertex, unsigned frag)
	{
		// shader Program
		m_program = glCreateProgram();
		glAttachShader(m_program, vertex);
		glAttachShader(m_program, frag);
		glLinkProgram(m_program);
		// print linking errors if any
		//CheckForError(m_program, GL_LINK_STATUS);
		// delete the shaders as they're linked into our program now and no longer necessary
		glDeleteShader(vertex);
		glDeleteShader(frag);
	}

}

