#include "Pch.h"
#include <Resources/Shader.h>
#include <cstdlib>
#include <spdlog/spdlog.h>
#if defined (PLATFORM_WINDOWS)
#include <glad/glad.h>
#elif defined (PLATFORM_SWITCH)

#endif
#include "Utility/StringUtility.h"

#include "Tools/ShaderPreprocessor.h"

namespace Frac
{
	void ShaderParameter::SetValue(float val) const
	{
		if (!IsValid())
			return;
		assert(m_type == GL_FLOAT);
		glUniform1f(m_location, val);
	}

	void ShaderParameter::SetValue(int val) const
	{
		if (!IsValid())
			return;

		assert(m_type == GL_INT);
		glUniform1i(m_location, val);
	}

	void ShaderParameter::SetValue(unsigned int val) const
	{
		if (!IsValid())
			return;
		assert(m_type == GL_UNSIGNED_INT);
		glUniform1ui(m_location, val);
	}

	void ShaderParameter::SetValue(bool val) const
	{
		if (!IsValid())
			return;
		assert(m_type == GL_BOOL);
		glUniform1i(m_location, val);
	}

	void ShaderParameter::SetValue(const glm::vec2& vec) const
	{
		if (!IsValid())
			return;
		assert(m_type == GL_FLOAT_VEC2);
		glUniform2fv(m_location, 1, &vec.x);
	}

	void ShaderParameter::SetValue(const glm::vec3& vec) const
	{
		if (!IsValid())
			return;
		assert(m_type == GL_FLOAT_VEC3);
		glUniform3fv(m_location, 1, &vec.x);

	}

	void ShaderParameter::SetValue(const glm::vec4& vec) const
	{
		if (!IsValid())
			return;

		// Colors can be represted as vec3 or vec4 in shaders
		if (m_type == GL_FLOAT_VEC4)
			glUniform4fv(m_location, 1, &vec.x);
		else if (m_type == GL_FLOAT_VEC3)
			glUniform3fv(m_location, 1, &vec.x);
		else
			assert(false);
	}

	void ShaderParameter::SetValue(const glm::mat3& mtx, bool transpose /*= false*/)
	{
		if (!IsValid())
			return;
		assert(m_type == GL_FLOAT_MAT3);
		glUniformMatrix3fv(m_location, 1, transpose, &mtx[0][0]);
	}

	void ShaderParameter::SetValue(const glm::mat4& mtx, bool transpose)
	{
		if (!IsValid())
			return;
		assert(m_type == GL_FLOAT_MAT4);
		glUniformMatrix4fv(m_location, 1, transpose, &mtx[0][0]);

	}

	// #TODO: implement textures.
	void ShaderParameter::SetValue(const Texture& texture)
	{
		if (!IsValid())
			return;

		assert(m_type == GL_SAMPLER_2D || m_type == GL_SAMPLER_2D_SHADOW);
		//glUniform1i(m_location, texture.GetSlot());
	}


	void ShaderAttribute::SetAttributePointer(GLint size,
		GLenum type,
		GLboolean normalized,
		GLsizei stride,
		const GLvoid* pointer)
	{
		if (!IsValid())
			return;

		glEnableVertexAttribArray(m_location);

		glVertexAttribPointer(
			m_location,          // attribute
			size,               // number of elements per vertex element
			type,               // the type of each element
			normalized,         // take our values as-is or normalize
			stride,             // no extra data between each position
			pointer             // offset of first element
		);
	}

	void ShaderAttribute::DisableAttributePointer()
	{
		if (!IsValid())
			return;

		glDisableVertexAttribArray(m_location);
	}


	Shader::Shader(const std::string& vertexFilename,
		const std::string& fragmentFilename)
		: Shader(vertexFilename, "", fragmentFilename)
	{}

	Shader::Shader(
		const std::string& vertexFilename,
		const std::string& geometryFilename,
		const std::string& fragmentFilename)
		: m_vertexFilename(vertexFilename)
		, m_fragmentFilename(fragmentFilename)
		, m_geometryFilename(geometryFilename)
		, m_computeFilename("")
	{
		m_resourcePath =
			geometryFilename.empty() ?
			vertexFilename + " " + fragmentFilename :
			vertexFilename + " " + geometryFilename + " " + fragmentFilename;

		Shader::Reload();
	}

	Shader::Shader(const std::string& computeFilename)
		: m_vertexFilename("")
		, m_fragmentFilename("")
		, m_geometryFilename("")
		, m_computeFilename(computeFilename)
	{
		m_resourcePath = computeFilename;

		Shader::Reload();
	}

	Shader::~Shader()
	{
		if (m_program > 0)
		{
			glDeleteProgram(m_program);
			m_program = 0;
		}
	}

	GLuint Shader::GetProgram() const
	{
		return m_program;
	}

	void Shader::Deactivate()
	{
		glUseProgram(0);
	}

	void Shader::Reload()
	{
		if (m_program > 0)
		{
			glDeleteProgram(m_program);
			m_program = 0;
		}

		bool success = Load();

		if (!success)
		{
			success = LoadMagentaShader();
			assert(success);
			spdlog::warn("Magenta shader loaded instead of " + m_resourcePath);
			LoadParameters();
		}

		if (!success)
		{
			spdlog::error("Unable to load shader {}", m_resourcePath.c_str());
		}
		else
		{
			LoadParameters();
		}
	}

	void Shader::LoadParameters()
	{
		// The shader should invalidate when reloading a new shader file
		// as some information can be old
		for (auto& itr : m_parameters)
			itr.second->Invalidate();
		for (auto& itr : m_attributes)
			itr.second->Invalidate();


		// Get the number of uniforms and resize the parameters collection accordingly
		GLint numActiveUniforms = 0;
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

		// Get the maximum name length
		GLint maxUniformNameLength = 0;
		glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
		assert(maxUniformNameLength < 512);

		//vector<GLchar> uniformNameData(maxUniformNameLength);

		GLchar uniformNameData[512];

		// Go over all the uniforms
		int samplerCount = 0;
		for (int uniform = 0; uniform < numActiveUniforms; uniform++)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			glGetActiveUniform(
				m_program,
				uniform,
				maxUniformNameLength,
				&actualLength,
				&arraySize,
				&type,
				&uniformNameData[0]);
			std::string name(&uniformNameData[0], actualLength);

			const GLint location = glGetUniformLocation(m_program, name.c_str());

			auto id = Utility::StringHash(name);
			auto itr = m_parameters.find(id);
			if (itr != m_parameters.end())
			{
				if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_SHADOW)
					itr->second->Reset(this, name, type, location, samplerCount++);
				else
					itr->second->Reset(this, name, type, location);
			}
			else
			{
				ShaderParameter* param = nullptr;
				if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_SHADOW)
					param = new ShaderParameter(this, name, type, location, samplerCount++);
				else
					param = new ShaderParameter(this, name, type, location);
				m_parameters[id] = std::unique_ptr<ShaderParameter>(param);
			}
		}


		GLint numActiveAttribs = 0;
		glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);


		GLint maxAttribNameLength = 0;
		glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
		std::vector<GLchar> attribNameData(maxAttribNameLength);

		GLuint numAttribs = (GLuint)numActiveAttribs;
		for (GLuint attrib = 0; attrib < numAttribs; ++attrib)
		{
			GLint arraySize = 0;
			GLenum type = 0;
			GLsizei actualLength = 0;
			glGetActiveAttrib(m_program,
				attrib,
				(GLsizei)attribNameData.size(),
				&actualLength,
				&arraySize,
				&type,
				&attribNameData[0]);
			std::string name(static_cast<char*>(&attribNameData[0]));
			GLint location = glGetAttribLocation(m_program, name.c_str());

			auto itr = m_attributes.find(name);
			if (itr != m_attributes.end())
			{
				itr->second->Reset(this, name, type, location);
			}
			else
			{
				ShaderAttribute* attribute = new ShaderAttribute(this, name, type, location);
				m_attributes[name] = std::unique_ptr<ShaderAttribute>(attribute);
			}
		}
	}

	ShaderParameter* Shader::GetParameter(const std::string& name)
	{
		const auto id = Utility::StringHash(name);
		return GetParameter(id);
	}

	ShaderParameter* Shader::GetParameter(unsigned long long nameId)
	{
		// Try to find param
		auto itr = m_parameters.find(nameId);
		if (itr != m_parameters.end())
			return itr->second.get();

		// Create and return a non-valid param that is stored in collection
		// in case it becomes valid after a reload
		ShaderParameter* param = new ShaderParameter();
		m_parameters[nameId] = std::unique_ptr<ShaderParameter>(param);
		return param;
	}


	ShaderAttribute* Shader::GetAttribute(const std::string& name)
	{
		// Try to find param
		auto itr = m_attributes.find(name);
		if (itr != m_attributes.end())
			return itr->second.get();

		// Create and return a non-valid param that is stored in collection
		// in case it becomes valid after a reload
		ShaderAttribute* attrib = new ShaderAttribute();
		m_attributes[name] = std::unique_ptr<ShaderAttribute>(attrib);
		return attrib;
	}

	void Shader::Activate()
	{
		glUseProgram(GetProgram());
	}

	bool Shader::Validate()
	{
	#if defined(_DEBUG)
		if (!ValidateProgram(m_program))
		{
			spdlog::error("Failed to validate program: {}", m_program);

			return false;
		}
	#endif
		return true;
	}

	bool Shader::Load()
	{
		static ShaderPreprocessor preprocessor;

		std::string vertShaderSource = "";
		if (m_vertexFilename.length() > 0)
			vertShaderSource = preprocessor.Read(m_vertexFilename);

		std::string geomShaderSource = "";
		if (m_geometryFilename.length() > 0)
			geomShaderSource = preprocessor.Read(m_geometryFilename);

		std::string fragShaderSource = "";
		if (m_fragmentFilename.length() > 0)
			fragShaderSource = preprocessor.Read(m_fragmentFilename);

		std::string computeShaderSource = "";
		if (m_computeFilename.length() > 0)
			computeShaderSource = preprocessor.Read(m_computeFilename);

		return LoadSource(vertShaderSource, geomShaderSource, fragShaderSource, computeShaderSource);
	}

	bool Shader::LoadMagentaShader()
	{
		std::string vertShaderSource =
			"#version 430 core\n \
			uniform mat4 u_projection; \
			uniform mat4 u_view; \
			uniform mat4 osm_model; \
			in vec3 a_position; \
			void main() \
			{ \
				gl_Position = u_projection * u_view * osm_model * vec4(a_position, 1.0); \
			}";
		std::string fragShaderSource =
			"#version 430 core\n \
			out vec4 fragColor; \
			void main() \
			{ \
				fragColor = vec4(1.0, 0.0, 1.0, 1.0); \
			}";

		return LoadSource(vertShaderSource, "", fragShaderSource, "");
	}

	std::vector<ShaderParameter*> Shader::GetAllParameters()
	{
		std::vector<ShaderParameter*> params;
		params.reserve(m_parameters.size());

		for (const auto& itr : m_parameters)
			params.push_back(itr.second.get());

		return params;
	}

	bool Shader::LoadSource(const std::string& vertexShader,
		const std::string& geometryShader,
		const std::string& fragmentShader,
		const std::string& computeShader)
	{
		GLuint vertShader = 0;
		GLuint fragShader = 0;
		GLuint geomShader = 0;
		GLuint compShader = 0;

		m_program = glCreateProgram();

		// Temp result
		GLboolean res;

		if (vertexShader.length() > 0)
		{
			res = CompileShader(&vertShader, GL_VERTEX_SHADER, vertexShader.c_str());
			if (!res)
			{
				spdlog::error("Shader::Load() Failed to compile vertex shader. File: {}", m_vertexFilename.c_str());
				return false;
			}
		}

		// Geometry shader is optional
		if (geometryShader.length() > 0)
		{
			res = CompileShader(&geomShader, GL_GEOMETRY_SHADER, geometryShader.c_str());
			if (!res)
			{
				spdlog::error("Shader::Load() Failed to compile geometry shader. File: {}", m_geometryFilename.c_str());
				return false;
			}
		}

		if (fragmentShader.length() > 0)
		{
			// Create and compile fragment shader
			res = CompileShader(&fragShader, GL_FRAGMENT_SHADER, fragmentShader.c_str());
			if (!res)
			{
				spdlog::error("Shader::Load() Failed to compile fragment shader. File: {}", m_fragmentFilename.c_str());
				return false;
			}
		}

		if (computeShader.length() > 0)
		{
			// Create and compile fragment shader
			res = CompileShader(&compShader, GL_COMPUTE_SHADER, computeShader.c_str());
			if (!res)
			{
				spdlog::error("Shader::Load() Failed to compile compute shader. File: {}", m_computeFilename.c_str());
				return false;
			}
		}

		// Attach vertex shader to program
		if (vertShader)
			glAttachShader(m_program, vertShader);

		// Attach geometry shader to program
		if (geomShader)
			glAttachShader(m_program, geomShader);

		// Attach fragment shader to program
		if (fragShader)
			glAttachShader(m_program, fragShader);

		// Attach compute shader to the program
		if (compShader)
			glAttachShader(m_program, compShader);

		// Link program
		if (!LinkProgram(m_program))
		{
			if (vertShader)
			{
				glDeleteShader(vertShader);
				vertShader = 0;
			}
			if (fragShader)
			{
				glDeleteShader(fragShader);
				fragShader = 0;
			}
			if (geomShader)
			{
				glDeleteShader(geomShader);
				geomShader = 0;
			}
			if (compShader)
			{
				glDeleteShader(compShader);
				compShader = 0;
			}
			if (m_program)
			{
				glDeleteProgram(m_program);
				m_program = 0;
			}

			spdlog::error("Shader::Load() Failed to link shader program.");
			return false;
		}

		glDeleteShader(vertShader);
		glDeleteShader(geomShader);
		glDeleteShader(fragShader);
		glDeleteShader(compShader);

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Compile shader and report success or failure
	////////////////////////////////////////////////////////////////////////////////
	bool Shader::CompileShader(GLuint* shader, GLenum type, const GLchar* source)
	{
		GLint status;

		if (!source)
		{
			spdlog::error("Failed to compile empty shader");
			return false;
		}

		*shader = glCreateShader(type);
		glShaderSource(*shader, 1, &source, nullptr);

		glCompileShader(*shader);

	#if defined(_DEBUG)
		GLint logLength = 0;
		glGetShaderiv(*shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 1)
		{
			GLchar* log = static_cast<GLchar*>(malloc(logLength));
			glGetShaderInfoLog(*shader, logLength, &logLength, log);
			if (log)
				spdlog::error("Program compile log: {}", log);
			free(log);
		}
	#endif

		glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);
		if (status == 0)
		{
			glDeleteShader(*shader);
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Link Shader Program
	////////////////////////////////////////////////////////////////////////////////
	bool Shader::LinkProgram(GLuint prog)
	{
		GLint status;

		glLinkProgram(prog);

	#if defined(_DEBUG)
		GLint logLength = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 1)
		{
			GLchar* log = static_cast<GLchar*>(malloc(logLength));
			glGetProgramInfoLog(prog, logLength, &logLength, log);
			if (log)
				spdlog::error("Program link log: {}", log);
			free(log);
		}
	#endif

		glGetProgramiv(prog, GL_LINK_STATUS, &status);
		return status != 0;
	}

	////////////////////////////////////////////////////////////////////////////////
	// Validate Shader Program
	////////////////////////////////////////////////////////////////////////////////
	bool Shader::ValidateProgram(GLuint prog)
	{
		GLint logLength = 0;
		GLint status = 0;

		glValidateProgram(prog);
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			GLchar* log = static_cast<GLchar*>(malloc(logLength));
			glGetProgramInfoLog(prog, logLength, &logLength, log);
			if (log)
				spdlog::error("Program validate log: {}", log);
			free(log);
		}

		logLength = 0;
		glGetProgramiv(prog, GL_VALIDATE_STATUS, &status);
		if (status == 0)
		{
			return false;
		}
		return true;
	}

	std::string Shader::GetPath(const std::string& vertexFilename, const std::string& fragmentFilename)
	{
		return vertexFilename + " " + fragmentFilename;
	}

	std::string Shader::GetPath(
		const std::string& vertexFilename,
		const std::string& geometryFilename,
		const std::string& fragmentFilename)
	{
		return vertexFilename + " " + geometryFilename + " " + fragmentFilename;
	}
}