#pragma once

namespace TOR
{
	class OpenGLShader;
	class OpenGLTexture;

	///
	/// OpenGLShaderParameter is a representation of an OpenGLShader parameter.
	/// It has a type and it will complain if the type declared in the
	/// OpenGLShader program is different.
	///
	class OpenGLShaderParameter
	{
		friend class OpenGLShader;

	public:
		/// Check if this parameter object is valid
		bool IsValid() const { return m_location != -1; }

		/// Gets the type of this parameter
		unsigned int GetType() const { return m_type; }

		/// Location of this parameter
		int GetLocation() const { return m_location; }

		/// Return the name of this parameter
		const std::string& GetName() const { return m_name; }

		/// Set the value. Parameter should be float.
		void SetValue(float val) const;

		/// Set the value. Parameter should be int.
		void SetValue(int val) const;

		/// Set the value. Parameter should be int.
		void SetValue(unsigned int val) const;

		/// Set the value. Parameter should be bool.
		void SetValue(bool val) const;

		/// Set the value. Parameter should be Vector2.
		void SetValue(const glm::vec2& vec) const;

		/// Set the value. Parameter should be Vector3.
		void SetValue(const glm::vec3& vec) const;

		/// Set the value. Parameter should be Vector4.
		void SetValue(const glm::vec4& vec) const;

		/// Set the value. Parameter should be Matrix33.
		void SetValue(const glm::mat3& mtx, bool transpose = false);

		/// Set the value. Parameter should be Matrix44.
		void SetValue(const glm::mat4& mtx, bool transpose = false);

		/// Set the value. Parameter should be Texture.
		void SetValue(const OpenGLTexture& texture);

	protected:

		/// The OpenGLShader creates a parameter.
		OpenGLShaderParameter(OpenGLShader* OpenGLShader, std::string name, unsigned int type, int location, int sampler = -1)
			: m_OpenGLShader(OpenGLShader)
			, m_name(name)
			, m_type(type)
			, m_location(location)
			, m_sampler(sampler)
		{}

		/// Constructor for an invalid OpenGLShaderParameter
		OpenGLShaderParameter() : m_OpenGLShader(nullptr), m_name(""), m_type(0), m_location(-1), m_sampler(-1) {}


		/// The OpenGLShader can reset the parameter after a reload
		void Reset(OpenGLShader* OpenGLShader, std::string name, unsigned int type, int location, int sampler = -1)
		{
			this->m_OpenGLShader = OpenGLShader;
			this->m_type = type;
			this->m_location = location;
			this->m_name = name;
			this->m_sampler = sampler;
		}

		/// The OpenGLShader should invalidate when reloading a new OpenGLShader file
		/// a some information can be old
		void Invalidate()
		{
			m_OpenGLShader = nullptr;
			m_name = "";
			m_type = 0;
			m_location = -1;
			m_sampler = -1;
		}

		/// The effect this parameter belongs to. Unused (beyond debugging)
		OpenGLShader* m_OpenGLShader;

		/// Name as defined in the OpenGLShader file.
		std::string	m_name;

		/// Type as defined in the OpenGLShader file.
		unsigned int m_type;

		/// Location for the setting for this.
		int	m_location;

		/// Only valid for type sampler (GL_SAMPLER_2D)
		int	m_sampler;
	};


	///
	/// A vertex attribute
	///
	class OpenGLShaderAttribute
	{
		friend class OpenGLShader;

	public:

		/// Check if this parameter object is valid
		bool IsValid() const { return _location != -1; }

		/// Gets the type of this parameter
		unsigned int GetType() const { return _type; }

		/// Location of this parameter
		int GetLocation() const { return _location; }

		/// Check documentation for glVertexAttribPointer
		void SetAttributePointer(
			int size,
			unsigned int type,
			unsigned char normalized,
			int stride,
			const void* pointer);

		/// Check documentation for glVertexAttribPointer
		void DisableAttributePointer();

	protected:

		/// The OpenGLShader creates a parameter.
		OpenGLShaderAttribute(OpenGLShader* OpenGLShader, std::string name, unsigned int type, int location) :
			_OpenGLShader(OpenGLShader)
			, _name(name)
			, _type(type)
			, _location(location)
		{}

		/// Constructor for an invalid OpenGLShaderParameter
		OpenGLShaderAttribute() : _OpenGLShader(nullptr), _name(""), _type(0), _location(-1) {}


		/// The OpenGLShader can reset the parameter after a reload
		void Reset(OpenGLShader* OpenGLShader, std::string name, unsigned type, int location)
		{
			this->_OpenGLShader = OpenGLShader;
			this->_type = type;
			this->_location = location;
			this->_name = name;
		}

		/// The OpenGLShader should invalidate when reloading a new OpenGLShader file
		/// as some information can be old
		void Invalidate()
		{
			_OpenGLShader = nullptr;
			_name = "";
			_type = 0;
			_location = -1;
		}


	protected:
		/// The effect this parameter belongs to. Unused (beyond debugging)
		OpenGLShader* _OpenGLShader;

		/// Name as defined in the OpenGLShader file.
		std::string  _name;

		/// Type as defined in the OpenGLShader file.
		unsigned	_type;

		/// Location for the setting for this.
		int			_location;

	};

	/// OpenGLShader is a class representing a compiled GPU program
	class OpenGLShader
	{
		friend class ResourceManager;

	public:
		/// Create an empty OpenGLShader. You will need to provide the source with LoadSource() before 
		/// you can use the OpenGLShader.
		OpenGLShader() = default;
		~OpenGLShader();

		/// Create a OpenGLShader with vertex and fragment programs 
		OpenGLShader(const std::string& vertexFilename,
			const std::string& fragmentFilename);

		/// Create a OpenGLShader with vertex geometry and fragment programs 
		OpenGLShader(const std::string& vertexFilename,
			const std::string& geometryFilename,
			const std::string& fragmentFilename);

		OpenGLShader(const std::string& computeFilename);

		OpenGLShader(const OpenGLShader& other) = delete;

		/// Load with source (as opposed to files)
		bool LoadSource(const std::string& vertexOpenGLShader,
			const std::string& geometryOpenGLShader,
			const std::string& fragmentOpenGLShader,
			const std::string& computeOpenGLShader);

		/// Get active parameter by name. If the parameter is not present/active
		/// you will get an invalid one.
		OpenGLShaderParameter* GetParameter(const std::string& name);

		/// Get active parameter by name id. If the parameter is not present/active
		/// you will get an invalid one.
		OpenGLShaderParameter* GetParameter(unsigned long long nameId);

		/// Get active attribute by name. If the attribute is not present/active
		/// you will get an invalid one.
		OpenGLShaderAttribute* GetAttribute(const std::string& name);

		/// Get the program in case you want to do everything manually
		unsigned int GetProgram() const;

		/// Call before drawing 
		void Activate();

		/// Call after drawing
		void Deactivate();

		std::vector<OpenGLShaderParameter*> GetAllParameters();

		void Reload() /*override*/;

		const std::string& GetVertexFilename() const { return _vertexFilename; }

		const std::string& GetFragmentFilename() const { return _fragmentFilename; }

		const std::string& GetGeometryFilename() const { return _geometryFilename; }

		void LoadParameters();

		static bool CompileShader(unsigned int* OpenGLShader, unsigned int type, const char* source);
		static bool LinkProgram(unsigned int prog);
		static bool ValidateProgram(unsigned int prog);

	private:

		static std::string GetPath(const std::string& vertexFilename,
			const std::string& fragmentFilename);

		static std::string GetPath(const std::string& vertexFilename,
			const std::string& geometryFilename,
			const std::string& fragmentFilename);

		bool Validate();

		/// Just a helper for the constructors
		bool Load();

		/// When loading fails, load magenta OpenGLShader
		bool LoadMagentaShader();

		std::string _resourcePath;

		/// Store all the parameters
		std::unordered_map<unsigned long long, std::unique_ptr<OpenGLShaderParameter>> _parameters;

		/// Store all the attributes
		std::unordered_map<std::string, std::unique_ptr<OpenGLShaderAttribute>> _attributes;

		/// Relative path to the vertex OpenGLShader
		std::string _vertexFilename;

		/// Relative path to the fragment OpenGLShader 
		std::string _fragmentFilename;

		/// Relative path to the geometry OpenGLShader
		std::string _geometryFilename;

		std::string _computeFilename;

		/// GL id (name) of the compiled program
		unsigned int _program = 0;
	};
}