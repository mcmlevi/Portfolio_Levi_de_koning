#include "pch.h"
#include "Tools/ShaderPreprocessor.h"
#include <regex>
#include <sstream>
#include <spdlog/spdlog.h>
#include "Utility/StringUtility.h"

#define ENABLE_PROFILING 0
#define ENABLE_CACHING 1

#if ENABLE_PROFILING
#endif

namespace {
	const std::regex sIncludeRegex = std::regex("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
} // anonymous namespace

namespace Frac
{
	ShaderPreprocessor::ShaderPreprocessor()
	{
		m_searchPaths.push_back("");
	}

	std::string ShaderPreprocessor::Read(const std::string& path)
	{
		std::set<std::string> includeTree;
		return ParseRecursive(path, "", includeTree);
	}

	// Based on
	// https://www.opengl.org/discussion_boards/showthread.php/169209-include-in-glsl
	std::string ShaderPreprocessor::ParseRecursive(
		const std::string& path,
		const std::string& parentPath,
		std::set<std::string>& includeTree)
	{
		std::string fullPath = parentPath.empty() ? path : parentPath + "/" + path;

		if (includeTree.count(fullPath))
		{
			spdlog::error("Circular include found! Path: {}", path.c_str());
			return std::string();
		}

		includeTree.insert(fullPath);

	#if ENABLE_CACHING
		if (Utility::StringEndsWith(fullPath, ".glsl"))
		{
			const auto cachedIt = m_cachedSources.find(path);
			if (cachedIt != m_cachedSources.end()) {
				return cachedIt->second;
			}
		}
	#endif	
		//#TODO: Wildcards.
		//fullPath = Engine.Resources().GetPath(fullPath);
		std::string parent = GetParentPath(fullPath);
		
		//#TODO: File reading.
		std::string inputString;// = ReadFileToString(fullPath);
		if (inputString.empty())
		{
			spdlog::error("Shader file not found! Path: {}", fullPath.c_str());
			return std::string();
		}

		std::stringstream input(std::move(inputString));
		std::stringstream output;

		// go through each line and process includes
		std::string line;
		std::smatch matches;
		size_t lineNumber = 1;
		std::string dbg;
		while (getline(input, line))
		{
			dbg = output.str();
			if (regex_search(line, matches, sIncludeRegex))
			{
				auto includeFile = ParseRecursive(matches[1].str(), parent, includeTree);
				output << includeFile;
				dbg = output.str();
				output << "#line " << lineNumber << std::endl;
				dbg = output.str();
			}
			else if (Utility::StringStartsWith(line, "#extension GL_GOOGLE_include_directive : require"))
			{
				output << std::endl;
			}
			else
			{
				if (!line.empty() && line[0] != '\0') // Don't null terminate
					output << line;
			}
			output << std::endl;
			lineNumber++;
			dbg = output.str();
		}

	#if ENABLE_CACHING
		if (Utility::StringEndsWith(fullPath, ".glsl"))
		{
			m_cachedSources[path] = output.str();
		}
	#endif

		std::string outputString = output.str();
		return outputString;
	}

	std::string ShaderPreprocessor::GetParentPath(const std::string& path)
	{
		std::string parent = "";
		std::string::size_type found = path.find_last_of("/");

		// if we found one of this symbols
		if (found != std::string::npos)
			parent = path.substr(0, found);

		return parent;
	}
}
