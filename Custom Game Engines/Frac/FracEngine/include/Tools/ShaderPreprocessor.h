#pragma once
#include <map>
#include <set>
#include <vector>
#include <string>

namespace Frac
{
	/// <summary>
	/// The preprocessor is used to parse GLSL shader source.
	/// </summary>
	class ShaderPreprocessor
	{
	public:
		ShaderPreprocessor();
		~ShaderPreprocessor() = default;

		ShaderPreprocessor(const ShaderPreprocessor& debugRenderer) = delete;
		void operator=(const ShaderPreprocessor& debugRenderer) = delete;

		std::string	Read(const std::string& path);

	private:
		std::string	ParseRecursive(const std::string& path,
			const std::string& parentPath,
			std::set<std::string>& includeTree);

		std::string	GetParentPath(const std::string& path);

		std::vector<std::string> m_searchPaths;
		std::map<std::string, std::string>	m_cachedSources;
	};
}