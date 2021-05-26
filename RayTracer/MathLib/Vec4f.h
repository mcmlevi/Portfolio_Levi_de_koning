#pragma once
#include "vec3f.h"
#include <cassert>
#pragma warning(push)
#pragma  warning(disable: 4201)
_MM_ALIGN16 class  Vec4f
{
public:
	// Constructors
	Vec4f(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) : m_coordinates{x,y,z,w} { }
	Vec4f(const Vec3f& xyz, float w = 0.f) : m_coordinates{ xyz.m_x,xyz.m_y,xyz.m_z,w } { }
	Vec4f(const __m128& input) :floats{input} {}
	union
	{
		__m128 floats;
		float m_coordinates[4];
		Vec3f m_XYZ;
	};


	
	


	float& operator[](const int rhs);
	const float& operator[](const int rhs) const;
	// Math operators
	Vec4f operator*(const Vec4f& rhs) const;

	Vec4f operator*(const float rhs) const;

	Vec4f operator-(const Vec4f& rhs) const;

	Vec4f operator+(const Vec4f& rhs) const;

	Vec4f operator/(const float rhs) const;

	// Assignment operators
	Vec4f& operator*=(const Vec4f& rhs);
	Vec4f& operator*=(const float rhs);
	Vec4f& operator/=(const float rhs);
	Vec4f& operator+=(const Vec4f& rhs);
	Vec4f& operator-=(const Vec4f& rhs);
	Vec4f& operator=(const Vec4f& rhs);

	// Conditional operators
	bool operator==(const Vec4f& rhs)const;
	bool operator!=(const Vec4f& rhs)const;

	// Vector specific functions
	float dot(const Vec4f& rhs) const;
	//Vec4f cross(const Vec4f rhs) const;
	//void clear();
	float lenght() const ;
	float lenght2() const;
	Vec4f& normalize();
	//Vec3f getNormalCopy()const;
	
};

inline Vec4f& Vec4f::operator=(const Vec4f& rhs)
{
	floats = rhs.floats;
	//m_coordinates[3] = rhs.m_coordinates[3];
	return  *this;
}

inline bool Vec4f::operator==(const Vec4f& rhs) const
{
	return   m_coordinates[0]== rhs.m_coordinates[0] && m_coordinates[1] == rhs.m_coordinates[1] && m_coordinates[2] == rhs.m_coordinates[2] && m_coordinates[3] == rhs.m_coordinates[3];
}

inline bool Vec4f::operator!=(const Vec4f& rhs) const
{
	return(!(*this == rhs));
}

inline float Vec4f::dot(const Vec4f& rhs) const
{
	//1111 1000
	//0111 1000
	return _mm_cvtss_f32(_mm_dp_ps(floats, rhs.floats, 241));
}

inline float Vec4f::lenght() const
{
	return (_mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(floats, floats, 241))));
}

inline float Vec4f::lenght2() const
{
	return _mm_cvtss_f32(_mm_dp_ps(floats, floats, 241));
}

inline Vec4f& Vec4f::normalize()
{
	floats = _mm_div_ps(floats , _mm_sqrt_ps(_mm_dp_ps(floats, floats, 0xFF)));
	return *this;
}

inline float& Vec4f::operator[](const int rhs)
{
	assert(rhs <= 3 && rhs>=0);
	return m_coordinates[rhs];
}

inline const float& Vec4f::operator[](const int rhs) const
{
	assert(rhs <= 3 && rhs >= 0);
	return m_coordinates[rhs];
}

inline Vec4f Vec4f::operator*(const Vec4f& rhs) const
{
	return _mm_mul_ps(floats, rhs.floats);
}

inline Vec4f Vec4f::operator*(const float rhs) const
{
	return _mm_mul_ps(floats,_mm_set_ps1(rhs));
}

inline Vec4f Vec4f::operator-(const Vec4f& rhs) const
{
	return _mm_sub_ps(floats, rhs.floats);
}

inline Vec4f Vec4f::operator+(const Vec4f& rhs) const
{
	return _mm_add_ps(floats, rhs.floats);
}

inline Vec4f Vec4f::operator/(const float rhs) const
{
	return _mm_div_ps(floats,_mm_set_ps1(rhs));
}

inline Vec4f& Vec4f::operator*=(const Vec4f& rhs)
{
	floats = _mm_mul_ps(floats, rhs.floats);
	return  *this;
}

inline Vec4f& Vec4f::operator*=(const float rhs)
{
	floats = _mm_mul_ps(floats, _mm_set_ps1(rhs));
	return  *this;
}

inline Vec4f& Vec4f::operator/=(const float rhs)
{
	floats = _mm_div_ps(floats, _mm_set_ps1(rhs));
	return  *this;
}

inline Vec4f& Vec4f::operator+=(const Vec4f& rhs)
{
	floats = _mm_add_ps(floats, rhs.floats);
	return *this;
}

inline Vec4f& Vec4f::operator-=(const Vec4f& rhs)
{
	floats = _mm_sub_ps(floats, rhs.floats);
	return *this;
}

inline Vec4f operator*(float lhs, const Vec4f& rhs)
{
	return _mm_mul_ps(rhs.floats, _mm_set_ps1(lhs));
}

#pragma warning(pop)
