#pragma once
#include <cstdio>
#include <string>
class LevelManager;
class IMapParser
{
public:
	IMapParser() {}
	virtual ~IMapParser() {}
	virtual void Parse(const std::string filepath) = 0;

};