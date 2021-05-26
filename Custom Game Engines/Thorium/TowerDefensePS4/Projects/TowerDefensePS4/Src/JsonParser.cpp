#include "pch.h"
#include "JsonParser.h"
#include "LevelManager.h"
void JsonParser::Parse(const std::string filepath)
{
	if (m_ActiveParsedFile != nullptr)
	{
		delete m_ActiveParsedFile;
		m_ActiveParsedFile = nullptr;
	}


	std::ifstream f(filepath);


	if (!f.is_open())
	{
		printf("Failed file loading at path:%s \n", filepath.c_str());
		return;
	}

	//create json object and store the file data in it
	json* j = new json;
	f >> *j;

	m_ActiveParsedFile = j;
	//std::ifstream f("testLevel.json");
}
