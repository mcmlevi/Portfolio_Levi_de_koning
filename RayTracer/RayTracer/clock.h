#pragma once
#include <ctime>

class Clock
{
public:
	Clock() { start = clock(); }
	double endTimer()
	{
		start = clock() - start;
		return static_cast<double>(start) / (CLOCKS_PER_SEC);
	}
private:
	
	clock_t start{};
	
};
