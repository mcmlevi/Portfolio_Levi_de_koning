#include <Thpch.h>
#include <Utility/Timer.h>

Th::Timer::Timer()
{
	ResetTimer();
}

void Th::Timer::ResetTimer()
{
	m_begin = std::chrono::high_resolution_clock::now();
}


using namespace std::chrono;
float Th::Timer::GetElapsedTime()
{
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(end - m_begin);
	return static_cast<float>(time_span.count());
}

float Th::Timer::GetElapsedTimeInMS()
{
	return GetElapsedTime() * 1000.f;
}

