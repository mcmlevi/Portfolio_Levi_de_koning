#pragma once
#include "cmath"
#include <cassert>
#include <xmmintrin.h>
#include <immintrin.h>



_MM_ALIGN16 class  Vec3f
{
public:
#pragma warning(push)
#pragma warning(disable : 4201 )
	union
	{
		__m128 SIMD;
		float m_coordinates[4];
		struct
		{
			float m_x;
			float m_y;
			float m_z;
			float m_w;
		};

	};

	// Constructors
	//Vec3f() :m_x{ 0.f }, m_y{ 0.f }, m_z{ 0.f } { }
	Vec3f(float x = 0.f, float y = 0.f, float z = 0.f) : SIMD{_mm_set_ps(0.f,z,y,x)} { }
	Vec3f(const __m128& input) :SIMD{input} {}
	//Vec3f(Vec3f& rhs) :floats{rhs.floats} {}
	  /// Overloaded operators that ensure alignment
	

	float& operator[](const int index);
	const float& operator[](const int index)const;
	
	// Math operators
	Vec3f operator*(const Vec3f& rhs) const;

	Vec3f operator*(const float rhs) const;

	Vec3f operator-(const Vec3f& rhs) const;

	Vec3f operator+(const Vec3f& rhs) const;

	Vec3f operator/(const float rhs) const;

	// Assignment operators
	Vec3f& operator*=(const Vec3f& rhs);
	Vec3f& operator*=(const float rhs);
	Vec3f& operator/=(const float rhs);
	Vec3f& operator+=(const Vec3f& rhs);
	Vec3f& operator-=(const Vec3f& rhs);
	Vec3f& operator=(const Vec3f& rhs);

	// Conditional operators
	bool operator==(const Vec3f& rhs)const;
	bool operator!=(const Vec3f& rhs)const;

	// Vector specific functions
	float dot(const Vec3f& rhs)const ;
	Vec3f cross(const Vec3f rhs) const;
	void clear();
	float lenght() const;
	float lenght2()const;
	Vec3f& normalize();
	Vec3f getNormalCopy();
};

inline float& Vec3f::operator[](const int index)
{
	assert(index <= 2 && index >= 0);
	return m_coordinates[index];
}

inline const float& Vec3f::operator[](const int index) const
{
	assert(index <= 2 && index >= 0);
	return m_coordinates[index];
}

inline Vec3f Vec3f::operator*(const Vec3f& rhs) const
{
	return _mm_mul_ps(this->SIMD, rhs.SIMD);
}

inline Vec3f Vec3f::operator*(const float rhs) const
{
	
	return _mm_mul_ps(this->SIMD, _mm_set_ps1(rhs));
}

inline Vec3f Vec3f::operator-(const Vec3f& rhs) const
{
	return _mm_sub_ps(SIMD,rhs.SIMD);
}

inline Vec3f Vec3f::operator+(const Vec3f& rhs) const
{
	return _mm_add_ps(SIMD, rhs.SIMD);
}

inline Vec3f Vec3f::operator/(const float rhs) const
{
	return _mm_div_ps(SIMD, _mm_set_ps1(rhs));
}

inline Vec3f& Vec3f::operator*=(const Vec3f& rhs)
{
	SIMD = _mm_mul_ps(this->SIMD, rhs.SIMD);
	return *this;
}

inline Vec3f& Vec3f::operator*=(const float rhs)
{
	SIMD = _mm_mul_ps(this->SIMD, _mm_set_ps1(rhs));
	return *this;
}

inline Vec3f& Vec3f::operator/=(const float rhs)
{
	SIMD = _mm_div_ps(SIMD, _mm_set_ps1(rhs));
	return *this;
}

inline Vec3f& Vec3f::operator+=(const Vec3f& rhs)
{
	SIMD = _mm_add_ps(SIMD, rhs.SIMD);
	return *this;
}

inline Vec3f& Vec3f::operator-=(const Vec3f& rhs)
{
	SIMD = _mm_sub_ps(SIMD, rhs.SIMD);
	return *this;
}

inline Vec3f& Vec3f::operator=(const Vec3f& rhs)
{
	SIMD = rhs.SIMD;
	return *this;
}

inline bool Vec3f::operator==(const Vec3f& rhs) const
{
	return (((_mm_movemask_ps(_mm_cmpeq_ps(SIMD,	rhs.SIMD))) & 0x7) == 0x7);
}

inline bool Vec3f::operator!=(const Vec3f& rhs) const
{
	return !(*this == rhs);
}
inline float Vec3f::dot(const Vec3f& rhs) const
{
	//return  m_x * rhs.m_x + m_y * rhs.m_y + m_z * rhs.m_z;
	//float result{};
	// 1110 1000
	// 0111 1000
	return _mm_cvtss_f32(_mm_dp_ps(SIMD, rhs.SIMD, 113));
}

inline Vec3f Vec3f::cross(const Vec3f rhs) const
{
	return _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(SIMD, SIMD, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(rhs.SIMD, rhs.SIMD, _MM_SHUFFLE(3, 1, 0, 2))),
		_mm_mul_ps(_mm_shuffle_ps(SIMD, SIMD, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(rhs.SIMD, rhs.SIMD, _MM_SHUFFLE(3, 0, 2, 1)))
	);
}

inline void Vec3f::clear()
{
	m_x = 0.f;
	m_y = 0.f;
	m_z = 0.f;
}

inline Vec3f operator*(float lhs, const Vec3f& rhs)
{
	return _mm_mul_ps(rhs.SIMD, _mm_set_ps1(lhs));
}
inline float Vec3f::lenght() const
{
	return (_mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(SIMD, SIMD, 113))));
}
inline float Vec3f::lenght2() const
{
	return (_mm_cvtss_f32(_mm_dp_ps(SIMD, SIMD, 113)));
}

inline Vec3f& Vec3f::normalize() 
{	
	SIMD = _mm_div_ps(SIMD, _mm_sqrt_ps(_mm_dp_ps(SIMD, SIMD, 127)));
	return *this;
}

inline Vec3f Vec3f::getNormalCopy() 
{
	float magnitude = this->lenght();
	return { m_x / magnitude,m_y / magnitude,m_z / magnitude };
}
#pragma warning(pop)