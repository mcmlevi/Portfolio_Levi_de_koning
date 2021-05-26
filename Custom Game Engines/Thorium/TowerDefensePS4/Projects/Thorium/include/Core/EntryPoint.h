#pragma once
#include <Core/Core.h>

extern Th::Core* Th::CreateGame();

int main()
{
	auto app = Th::CreateGame();
	app->Initialize();
	delete app;
}
