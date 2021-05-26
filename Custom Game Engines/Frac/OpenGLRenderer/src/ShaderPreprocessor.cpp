#include "OpenGLRenderer/pch.h"
#include "OpenGLRenderer/ShaderPreprocessor.h"


#define ENABLE_PROFILING 0
#define ENABLE_CACHING 1

#if ENABLE_PROFILING
#endif

using namespace std;

namespace {
	const regex sIncludeRegex = regex("^[ ]*#[ ]*include[ ]+[\"<](.*)[\">].*");
} // anonymous namespace

namespace TOR
{

ShaderPreprocessor::ShaderPreprocessor()
{
	m_searchPaths.push_back("");
}

string ShaderPreprocessor::Read(const string& path)
{
	set<string> includeTree;
	return ParseRecursive(path, "", includeTree);
}

// Based on
// https://www.opengl.org/discussion_boards/showthread.php/169209-include-in-glsl
string ShaderPreprocessor::ParseRecursive(
	const string& path,
	const string& parentPath,
	set<string>& includeTree)
{
	string fullPath = parentPath.empty() ? path : parentPath + "/" + path;

	if (includeTree.count(fullPath))
	{
		spdlog::error("Circular include found! Path: {}", path.c_str());
		return string();
	}

	includeTree.insert(fullPath);

#if ENABLE_CACHING
	if (TOR::StringEndsWith(fullPath, ".glsl"))
	{
		const auto cachedIt = m_cachedSources.find(path);
		if (cachedIt != m_cachedSources.end()) {
			return cachedIt->second;
		}
	}
#endif	
	//	#TODO: make something that manages wildcards?
		//Engine.Resources().GetPath(fullPath);
	string parent = GetParentPath(fullPath);
	string inputString = ReadFileToString(fullPath);
	if (inputString.empty())
	{
		spdlog::error("Shader file not found! Path: {}", fullPath.c_str());
		return string();
	}

	stringstream input(move(inputString));
	stringstream output;

	// go through each line and process includes
	string line;
	smatch matches;
	size_t lineNumber = 1;
	while (getline(input, line))
	{
		if (regex_search(line, matches, sIncludeRegex))
		{
			output << ParseRecursive(matches[1].str(), parent, includeTree);
			output << "#line " << lineNumber << endl;
		}
		else if (TOR::StringStartsWith(line, "#extension GL_GOOGLE_include_directive : require"))
		{
			output << std::endl;
		}
		else
		{
			if (!line.empty() && line[0] != '\0') // Don't null terminate
				output << line;
		}
		output << endl;
		lineNumber++;
	}

#if ENABLE_CACHING
	if (TOR::StringEndsWith(fullPath, ".glsl"))
	{
		m_cachedSources[path] = output.str();
	}
#endif
	return output.str();

}

string ShaderPreprocessor::GetParentPath(const string& path)
{
	// Implementation base on:
	// http://stackoverflow.com/questions/28980386/how-to-get-file-name-from-a-whole-path-by-c

	string parent = "";
	string::size_type found = path.find_last_of("/");

	// if we found one of this symbols
	if (found != string::npos)
		parent = path.substr(0, found);

	return parent;
}
}