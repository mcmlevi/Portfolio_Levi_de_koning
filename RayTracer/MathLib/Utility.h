#pragma once
#include "math.h"
constexpr float epsilon{ 0.0001f };
constexpr float M_PI{ 3.141592653589793238f };
template<typename T>
void swap(T& a, T& b)
{
	T temp = a;
	a = b;
	b = temp;
}

template<typename T>
T max(const T& a, const T& b)
{
	if (b < a)
		return  a;
	else
		return  b;
}
template<typename T>
T min(const T& a, const T& b)
{
	if (b > a)
		return  a;
	else
		return  b;
}
template<typename T>
T clamp(const T& a, const T& min, const T& max)
{
	if (a < min)
		return  min;
	if (a > max)
		return  max;
	else
		return a;
}
inline float degreesToRads(const float& degrees)
{
	return degrees * (M_PI / 180.f);
}


inline bool solveQuadratic(const float& a, const float& b, const float& c, float& x1, float& x2)
{
	float discriminant{ b * b - (4 * (a * c)) };
	// We only hit the object on 1 point
	if (discriminant <= epsilon && discriminant >= 0)
	{
		// sqrt(0) == 0 so we can shorten the calculation.
		x1 = (-b / (2 * a));
		x2 = x1;
		return  true;
	}
	// We don't hit the object
	else if (discriminant < 0)
	{
		return  false;
	}
	// We hit the object on 2 points
	else
	{
		float root{ sqrtf(discriminant) };
		x1 = (-b + root) / (2.f * a);
		x2 = (-b - root) / (2.f * a);
		if (x1 < 0 && x2 < 0)
			return false;

		if (x1 > x2)
			swap(x1, x2);
		return true;
	}
}
template <typename  T>
T distanceBetween(T a, T b)
{
	return abs(a - b);
}