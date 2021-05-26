#pragma once
#include <Core/EntryPoint.h>

class TBRGame : public Th::Core
{
public:
	TBRGame() : Th::Core() {}
	~TBRGame() = default;

	void Initialize();
	void Shutdown();
};