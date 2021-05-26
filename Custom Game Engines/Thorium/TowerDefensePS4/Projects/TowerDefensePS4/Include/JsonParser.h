#pragma once
#ifndef JSONPARSER_H
#define JSONPARSER_H
#include <Nlohmann/Include/json.hpp>
#include "IMapParser.h"
#include "fstream"


using json = nlohmann::json;

class JsonParser : public IMapParser
{
public:
	JsonParser() = default;
	~JsonParser() = default;
	void Parse(const std::string filepath) override;
	json* m_ActiveParsedFile = nullptr;
};
#endif

