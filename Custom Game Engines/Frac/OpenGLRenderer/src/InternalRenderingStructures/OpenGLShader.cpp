#include "OpenGLRenderer/pch.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLShader.h"
#include "OpenGLRenderer/InternalRenderingStructures/OpenGLTexture.h"
#include "OpenGLRenderer/ShaderPreprocessor.h"


using namespace std;
using namespace TOR;

////////////////////////////////////////////////////////////////////////////////
//
//                              OpenGLShaderParameter
//
////////////////////////////////////////////////////////////////////////////////

void OpenGLShaderParameter::SetValue(float val) const
{
	if (!IsValid())
		return;
	assert(m_type == GL_FLOAT);
	glUniform1f(m_location, val);
}


void OpenGLShaderParameter::SetValue(int val) const
{
	if (!IsValid())
		return;

	//assert(m_type == GL_INT);
	glUniform1i(m_location, val);
}

void OpenGLShaderParameter::SetValue(unsigned int val) const
{
	if (!IsValid())
		return;
	assert(m_type == GL_UNSIGNED_INT);
	glUniform1ui(m_location, val);
}

void OpenGLShaderParameter::SetValue(bool val) const
{
	if (!IsValid())
		return;
	assert(m_type == GL_BOOL);
	glUniform1i(m_location, val);
}

void OpenGLShaderParameter::SetValue(const glm::vec2& vec) const
{
	if (!IsValid())
		return;
	assert(m_type == GL_FLOAT_VEC2);
	glUniform2fv(m_location, 1, &vec.x);
}

void OpenGLShaderParameter::SetValue(const glm::vec3& vec) const
{
	if (!IsValid())
		return;
	assert(m_type == GL_FLOAT_VEC3);
	glUniform3fv(m_location, 1, &vec.x);

}

void OpenGLShaderParameter::SetValue(const glm::vec4& vec) const
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

void OpenGLShaderParameter::SetValue(const glm::mat3& mtx, bool transpose /*= false*/)
{
	if (!IsValid())
		return;
	assert(m_type == GL_FLOAT_MAT3);
	glUniformMatrix3fv(m_location, 1, transpose, glm::value_ptr(mtx));
}

void OpenGLShaderParameter::SetValue(const glm::mat4& mtx, bool transpose)
{
	if (!IsValid())
		return;
	assert(m_type == GL_FLOAT_MAT4);
	glUniformMatrix4fv(m_location, 1, transpose, glm::value_ptr(mtx));

}



// #TODO: implement textures.
void OpenGLShaderParameter::SetValue(const OpenGLTexture& texture)
{
	if (!IsValid())
		return;

	assert(m_type == GL_SAMPLER_2D || m_type == GL_SAMPLER_2D_SHADOW);
	glUniform1i(m_location, texture.GetSlot());
}


////////////////////////////////////////////////////////////////////////////////
//
//                          OpenGLShaderAttribute
//
////////////////////////////////////////////////////////////////////////////////

void OpenGLShaderAttribute::SetAttributePointer(GLint size,
	GLenum type,
	GLboolean normalized,
	GLsizei stride,
	const GLvoid* pointer)
{
	if (!IsValid())
		return;

	glEnableVertexAttribArray(_location);

	glVertexAttribPointer(
		_location,          // attribute
		size,               // number of elements per vertex element
		type,               // the type of each element
		normalized,         // take our values as-is or normalize
		stride,             // no extra data between each position
		pointer             // offset of first element
	);
}

void OpenGLShaderAttribute::DisableAttributePointer()
{
	if (!IsValid())
		return;

	glDisableVertexAttribArray(_location);
}


////////////////////////////////////////////////////////////////////////////////
//
//                                  Shader
//
////////////////////////////////////////////////////////////////////////////////

OpenGLShader::OpenGLShader(const std::string& vertexFilename,
	const std::string& fragmentFilename)
	: OpenGLShader(vertexFilename, "", fragmentFilename)
{}

OpenGLShader::OpenGLShader(
	const std::string& vertexFilename,
	const std::string& geometryFilename,
	const std::string& fragmentFilename)
	: _vertexFilename(vertexFilename)
	, _fragmentFilename(fragmentFilename)
	, _geometryFilename(geometryFilename)
	, _computeFilename("")
{
	_resourcePath =
		geometryFilename.empty() ?
		vertexFilename + " " + fragmentFilename :
		vertexFilename + " " + geometryFilename + " " + fragmentFilename;

	OpenGLShader::Reload();
}

TOR::OpenGLShader::OpenGLShader(const std::string& computeFilename)
	: _vertexFilename("")
	, _fragmentFilename("")
	, _geometryFilename("")
	, _computeFilename(computeFilename)
{
	_resourcePath = computeFilename;

	OpenGLShader::Reload();
}

OpenGLShader::~OpenGLShader()
{
	if (_program > 0)
	{
		glDeleteProgram(_program);
		_program = 0;
	}
}

GLuint OpenGLShader::GetProgram() const
{
	return _program;
}

void OpenGLShader::Deactivate()
{
	glUseProgram(0);
}

void OpenGLShader::Reload()
{
	if (_program > 0)
	{
		glDeleteProgram(_program);
		_program = 0;
	}

	bool success = Load();

	if (!success)
	{
		success = LoadMagentaShader();
		assert(success);
		spdlog::warn("Magenta shader loaded instead of " + _resourcePath);
		LoadParameters();
	}

	if (!success)
	{
		spdlog::error("Unable to load shader {}", _resourcePath.c_str());
	}
	else
	{
		LoadParameters();
	}
}

void OpenGLShader::LoadParameters()
{
	// The shader should invalidate when reloading a new shader file
	// as some information can be old
	for (auto& itr : _parameters)
		itr.second->Invalidate();
	for (auto& itr : _attributes)
		itr.second->Invalidate();


	// Get the number of uniforms and resize the parameters collection accordingly
	GLint numActiveUniforms = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numActiveUniforms);

	// Get the maximum name length
	GLint maxUniformNameLength = 0;
	glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformNameLength);
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
			_program,
			uniform,
			maxUniformNameLength,
			&actualLength,
			&arraySize,
			&type,
			&uniformNameData[0]);
		string name(&uniformNameData[0], actualLength);

		const GLint location = glGetUniformLocation(_program, name.c_str());

		auto id = TOR::StringHash(name);
		auto itr = _parameters.find(id);
		if (itr != _parameters.end())
		{
			if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_SHADOW)
				itr->second->Reset(this, name, type, location, samplerCount++);
			else
				itr->second->Reset(this, name, type, location);
		}
		else
		{
			OpenGLShaderParameter* param = nullptr;
			if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE || type == GL_SAMPLER_2D_SHADOW)
				param = new OpenGLShaderParameter(this, name, type, location, samplerCount++);
			else
				param = new OpenGLShaderParameter(this, name, type, location);
			_parameters[id] = unique_ptr<OpenGLShaderParameter>(param);
		}
	}


	GLint numActiveAttribs = 0;
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numActiveAttribs);


	GLint maxAttribNameLength = 0;
	glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribNameLength);
	vector<GLchar> attribNameData(maxAttribNameLength);

	GLuint numAttribs = (GLuint)numActiveAttribs;
	for (GLuint attrib = 0; attrib < numAttribs; ++attrib)
	{
		GLint arraySize = 0;
		GLenum type = 0;
		GLsizei actualLength = 0;
		glGetActiveAttrib(_program,
			attrib,
			(GLsizei)attribNameData.size(),
			&actualLength,
			&arraySize,
			&type,
			&attribNameData[0]);
		std::string name(static_cast<char*>(&attribNameData[0]));
		GLint location = glGetAttribLocation(_program, name.c_str());

		auto itr = _attributes.find(name);
		if (itr != _attributes.end())
		{
			itr->second->Reset(this, name, type, location);
		}
		else
		{
			OpenGLShaderAttribute* attribute = new OpenGLShaderAttribute(this, name, type, location);
			_attributes[name] = unique_ptr<OpenGLShaderAttribute>(attribute);
		}
	}
}

OpenGLShaderParameter* OpenGLShader::GetParameter(const string& name)
{
	const auto id = TOR::StringHash(name);
	return GetParameter(id);
}

OpenGLShaderParameter* OpenGLShader::GetParameter(unsigned long long nameId)
{
	// Try to find param
	auto itr = _parameters.find(nameId);
	if (itr != _parameters.end())
		return itr->second.get();

	// Create and return a non-valid param that is stored in collection
	// in case it becomes valid after a reload
	OpenGLShaderParameter* param = new OpenGLShaderParameter();
	_parameters[nameId] = unique_ptr<OpenGLShaderParameter>(param);
	return param;
}


OpenGLShaderAttribute* OpenGLShader::GetAttribute(const string& name)
{
	// Try to find param
	auto itr = _attributes.find(name);
	if (itr != _attributes.end())
		return itr->second.get();

	// Create and return a non-valid param that is stored in collection
	// in case it becomes valid after a reload
	OpenGLShaderAttribute* attrib = new OpenGLShaderAttribute();
	_attributes[name] = unique_ptr<OpenGLShaderAttribute>(attrib);
	return attrib;
}

void OpenGLShader::Activate()
{
	glUseProgram(GetProgram());
}

bool OpenGLShader::Validate()
{
#if defined(_DEBUG)
	if (!ValidateProgram(_program))
	{
		spdlog::error("Failed to validate program: {}", _program);

		return false;
	}
#endif
	return true;
}

bool OpenGLShader::Load()
{
	static ShaderPreprocessor preprocessor;

	string vertShaderSource = "";
	if (_vertexFilename.length() > 0)
		vertShaderSource = preprocessor.Read(_vertexFilename);

	string geomShaderSource = "";
	if (_geometryFilename.length() > 0)
		geomShaderSource = preprocessor.Read(_geometryFilename);

	string fragShaderSource = "";
	if (_fragmentFilename.length() > 0)
		fragShaderSource = preprocessor.Read(_fragmentFilename);

	string computeShaderSource = "";
	if (_computeFilename.length() > 0)
		computeShaderSource = preprocessor.Read(_computeFilename);

	return LoadSource(vertShaderSource, geomShaderSource, fragShaderSource, computeShaderSource);
}

bool OpenGLShader::LoadMagentaShader()
{
	string vertShaderSource =
		"#version 430 core\n \
		uniform mat4 u_projection; \
		uniform mat4 u_view; \
		uniform mat4 osm_model; \
		in vec3 a_position; \
		void main() \
		{ \
			gl_Position = u_projection * u_view * osm_model * vec4(a_position, 1.0); \
		}";
	string fragShaderSource =
		"#version 430 core\n \
		out vec4 fragColor; \
		void main() \
		{ \
			fragColor = vec4(1.0, 0.0, 1.0, 1.0); \
		}";

	return LoadSource(vertShaderSource, "", fragShaderSource, "");
}

vector<OpenGLShaderParameter*> OpenGLShader::GetAllParameters()
{
	vector<OpenGLShaderParameter*> params;
	params.reserve(_parameters.size());

	for (const auto& itr : _parameters)
		params.push_back(itr.second.get());

	return params;
}

bool OpenGLShader::LoadSource(const string& vertexShader,
	const string& geometryShader,
	const string& fragmentShader,
	const string& computeShader)
{
	GLuint vertShader = 0;
	GLuint fragShader = 0;
	GLuint geomShader = 0;
	GLuint compShader = 0;

	_program = glCreateProgram();

	// Temp result
	GLboolean res;

	if (vertexShader.length() > 0)
	{
		res = CompileShader(&vertShader, GL_VERTEX_SHADER, vertexShader.c_str());
		if (!res)
		{
			spdlog::error("Shader::Load() Failed to compile vertex shader. File: {}", _vertexFilename.c_str());
			return false;
		}
	}

	// Geometry shader is optional
	if (geometryShader.length() > 0)
	{
		res = CompileShader(&geomShader, GL_GEOMETRY_SHADER, geometryShader.c_str());
		if (!res)
		{
			spdlog::error("Shader::Load() Failed to compile geometry shader. File: {}", _geometryFilename.c_str());
			return false;
		}
	}

	if (fragmentShader.length() > 0)
	{
		// Create and compile fragment shader
		res = CompileShader(&fragShader, GL_FRAGMENT_SHADER, fragmentShader.c_str());
		if (!res)
		{
			spdlog::error("Shader::Load() Failed to compile fragment shader. File: {}", _fragmentFilename.c_str());
			return false;
		}
	}

	if (computeShader.length() > 0)
	{
		// Create and compile fragment shader
		res = CompileShader(&compShader, GL_COMPUTE_SHADER, computeShader.c_str());
		if (!res)
		{
			spdlog::error("Shader::Load() Failed to compile compute shader. File: {}", _computeFilename.c_str());
			return false;
		}
	}

	// Attach vertex shader to program
	if (vertShader)
		glAttachShader(_program, vertShader);

	// Attach geometry shader to program
	if (geomShader)
		glAttachShader(_program, geomShader);

	// Attach fragment shader to program
	if (fragShader)
		glAttachShader(_program, fragShader);

	// Attach compute shader to the program
	if (compShader)
		glAttachShader(_program, compShader);

	// Link program
	if (!LinkProgram(_program))
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
		if (_program)
		{
			glDeleteProgram(_program);
			_program = 0;
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
bool OpenGLShader::CompileShader(GLuint* shader, GLenum type, const GLchar* source)
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
bool OpenGLShader::LinkProgram(GLuint prog)
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
bool OpenGLShader::ValidateProgram(GLuint prog)
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

std::string OpenGLShader::GetPath(const std::string& vertexFilename, const std::string& fragmentFilename)
{
	return vertexFilename + " " + fragmentFilename;
}

std::string OpenGLShader::GetPath(
	const std::string& vertexFilename,
	const std::string& geometryFilename,
	const std::string& fragmentFilename)
{
	return vertexFilename + " " + geometryFilename + " " + fragmentFilename;
}
