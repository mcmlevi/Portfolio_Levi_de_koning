#pragma once
#include <SFML/Graphics.hpp>
#pragma warning(push)
#pragma warning(disable : 4201)
struct Color
{
	Color(sf::Uint8 r, sf::Uint8 g, sf::Uint8 b) :R{ (float)(r) / 255.f }, G{ (float)(g) / 255.f }, B{ (float)(b) / 255.f }  {}
	Color(float r,float g, float b):R(r),G(g),B(b){}

	Color() :R{ 0 }, G{ 0 }, B{ 0 } {}
	
	union
	{
		float colorInfo[3];
		struct
		{
			float R;
			float G;
			float B;
		};
	};

	Color operator*(const float& rhs);
	Color operator* (const Color& rhs);
	Color& operator+=(const Color& rhs);
	Color operator+(const Color& rhs);
	void clamp();
	float clamp(const float& rhs);
	
	
};

inline Color Color::operator*(const float& rhs)
{
	return Color{ R*rhs,G*rhs,B*rhs };
}

inline Color Color::operator*(const Color& rhs)
{
	return Color{ R * rhs.R, G * rhs.G,B * rhs.B };
}

inline Color& Color::operator+=(const Color& rhs)
{
	this->R += rhs.R;
	this->G += rhs.G;
	this->B += rhs.B;
	//this->A += rhs.A;
	return  *this;
}

inline Color Color::operator+(const Color& rhs)
{
	return { R + rhs.R,G + rhs.G, B + rhs.B };
}


inline void Color::clamp()
{
	R = clamp(R);
	G = clamp(G);
	B = clamp(B);
	
}

inline float Color::clamp(const float& rhs)
{
	return rhs < 0.f ? 0.f : rhs>1.f ? 1.f : rhs;
}

#pragma warning(pop)
