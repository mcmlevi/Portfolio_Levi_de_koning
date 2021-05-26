// TDGame.cpp : Defines the entry point to the NintendoSDK application.
//

#include "pch.h"
#include "Game.h"

#if defined(PLATFORM_SWITCH)

#elif defined(PLATFORM_WINDOWS) 
int main(int argc, char* argv[])
#endif
{
	std::cout << "It's alive!" << std::endl;
	std::unique_ptr<Game> game = std::make_unique<Game>();
}
