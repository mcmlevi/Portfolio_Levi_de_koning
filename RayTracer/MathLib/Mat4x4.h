#pragma once
#include "Vec4f.h"
#include "Utility.h"


#pragma warning(push)
#pragma  warning(disable: 4201)
class Mat4x4
{
public:
	Mat4x4() :m_X{1.f,0.f,0.f,0.f}, m_Y{0.f,1.f,0.0f,0.f}, m_Z{0.f,0.f,1.f,0.f}, m_W{0.f,0.f,0.f,1.f} {}
	Mat4x4(Vec4f x, Vec4f y, Vec4f z) :m_X{ x }, m_Y{ y }, m_Z{ z }, m_W{}{}
	Mat4x4(Vec4f x, Vec4f y, Vec4f z, Vec4f w) :m_X{ x }, m_Y{ y }, m_Z{ z }, m_W{ w }{}
	Mat4x4(__m128 x, __m128 y, __m128 z, __m128 w) :m_X{ x }, m_Y{ y }, m_Z{ z }, m_W{ w }{}
	Mat4x4(__m128 rows[4]) :m_X{ rows[0] }, m_Y{ rows[1] }, m_Z{ rows[2] }, m_W{ rows[3] }{}

	union
	{
		Vec4f rows[4];
		struct
		{
			Vec4f m_X;
			Vec4f m_Y;
			Vec4f m_Z;
			Vec4f m_W;
		};
		//__m128 simdRows[4];
		float elements[16];
	};

	const Vec4f& operator[](const int index) const;
	Vec4f& operator[](const int index);
	// Math operators
	Vec4f operator*(const Vec4f& rhs);
	Mat4x4 operator*(const float& scalar) const;
	Mat4x4 operator+(const Mat4x4& rhs) const;
	Mat4x4& operator+=(const Mat4x4& rhs);
	Mat4x4 operator-(const Mat4x4& rhs) const;
	Mat4x4& operator-=(const Mat4x4& rhs);
	Mat4x4 operator*(const Mat4x4& rhs);
	Mat4x4& operator*=(const Mat4x4& rhs);

	Mat4x4& operator=(const Mat4x4& rhs);
	bool inverse();
	float determinant();
	void transpose();
	Vec3f getTranslation();
	void setTranslation(const Vec3f& pos);
	Mat4x4 rotateX(float degrees);
	Mat4x4 rotateY(float degrees);
	Mat4x4 rotateZ(float degrees);
	Mat4x4& scaleUniform( float scale);
	Mat4x4& scale(const Vec3f& scale);

};

inline bool Mat4x4::inverse()
{
	float inv[16];
	inv[0] = elements[5] * elements[10] * elements[15] -
		elements[5] * elements[11] * elements[14] -
		elements[9] * elements[6] * elements[15] +
		elements[9] * elements[7] * elements[14] +
		elements[13] * elements[6] * elements[11] -
		elements[13] * elements[7] * elements[10];

	inv[4] = -elements[4] * elements[10] * elements[15] +
		elements[4] * elements[11] * elements[14] +
		elements[8] * elements[6] * elements[15] -
		elements[8] * elements[7] * elements[14] -
		elements[12] * elements[6] * elements[11] +
		elements[12] * elements[7] * elements[10];

	inv[8] = elements[4] * elements[9] * elements[15] -
		elements[4] * elements[11] * elements[13] -
		elements[8] * elements[5] * elements[15] +
		elements[8] * elements[7] * elements[13] +
		elements[12] * elements[5] * elements[11] -
		elements[12] * elements[7] * elements[9];

	inv[12] = -elements[4] * elements[9] * elements[14] +
		elements[4] * elements[10] * elements[13] +
		elements[8] * elements[5] * elements[14] -
		elements[8] * elements[6] * elements[13] -
		elements[12] * elements[5] * elements[10] +
		elements[12] * elements[6] * elements[9];

	inv[1] = -elements[1] * elements[10] * elements[15] +
		elements[1] * elements[11] * elements[14] +
		elements[9] * elements[2] * elements[15] -
		elements[9] * elements[3] * elements[14] -
		elements[13] * elements[2] * elements[11] +
		elements[13] * elements[3] * elements[10];

	inv[5] = elements[0] * elements[10] * elements[15] -
		elements[0] * elements[11] * elements[14] -
		elements[8] * elements[2] * elements[15] +
		elements[8] * elements[3] * elements[14] +
		elements[12] * elements[2] * elements[11] -
		elements[12] * elements[3] * elements[10];

	inv[9] = -elements[0] * elements[9] * elements[15] +
		elements[0] * elements[11] * elements[13] +
		elements[8] * elements[1] * elements[15] -
		elements[8] * elements[3] * elements[13] -
		elements[12] * elements[1] * elements[11] +
		elements[12] * elements[3] * elements[9];

	inv[13] = elements[0] * elements[9] * elements[14] -
		elements[0] * elements[10] * elements[13] -
		elements[8] * elements[1] * elements[14] +
		elements[8] * elements[2] * elements[13] +
		elements[12] * elements[1] * elements[10] -
		elements[12] * elements[2] * elements[9];

	inv[2] = elements[1] * elements[6] * elements[15] -
		elements[1] * elements[7] * elements[14] -
		elements[5] * elements[2] * elements[15] +
		elements[5] * elements[3] * elements[14] +
		elements[13] * elements[2] * elements[7] -
		elements[13] * elements[3] * elements[6];

	inv[6] = -elements[0] * elements[6] * elements[15] +
		elements[0] * elements[7] * elements[14] +
		elements[4] * elements[2] * elements[15] -
		elements[4] * elements[3] * elements[14] -
		elements[12] * elements[2] * elements[7] +
		elements[12] * elements[3] * elements[6];

	inv[10] = elements[0] * elements[5] * elements[15] -
		elements[0] * elements[7] * elements[13] -
		elements[4] * elements[1] * elements[15] +
		elements[4] * elements[3] * elements[13] +
		elements[12] * elements[1] * elements[7] -
		elements[12] * elements[3] * elements[5];

	inv[14] = -elements[0] * elements[5] * elements[14] +
		elements[0] * elements[6] * elements[13] +
		elements[4] * elements[1] * elements[14] -
		elements[4] * elements[2] * elements[13] -
		elements[12] * elements[1] * elements[6] +
		elements[12] * elements[2] * elements[5];

	inv[3] = -elements[1] * elements[6] * elements[11] +
		elements[1] * elements[7] * elements[10] +
		elements[5] * elements[2] * elements[11] -
		elements[5] * elements[3] * elements[10] -
		elements[9] * elements[2] * elements[7] +
		elements[9] * elements[3] * elements[6];

	inv[7] = elements[0] * elements[6] * elements[11] -
		elements[0] * elements[7] * elements[10] -
		elements[4] * elements[2] * elements[11] +
		elements[4] * elements[3] * elements[10] +
		elements[8] * elements[2] * elements[7] -
		elements[8] * elements[3] * elements[6];

	inv[11] = -elements[0] * elements[5] * elements[11] +
		elements[0] * elements[7] * elements[9] +
		elements[4] * elements[1] * elements[11] -
		elements[4] * elements[3] * elements[9] -
		elements[8] * elements[1] * elements[7] +
		elements[8] * elements[3] * elements[5];

	inv[15] = elements[0] * elements[5] * elements[10] -
		elements[0] * elements[6] * elements[9] -
		elements[4] * elements[1] * elements[10] +
		elements[4] * elements[2] * elements[9] +
		elements[8] * elements[1] * elements[6] -
		elements[8] * elements[2] * elements[5];

	float det = elements[0] * inv[0] + elements[1] * inv[4] + elements[2] * inv[8] + elements[3] * inv[12];

	if (det == 0)
		return false;

	det = 1.0f / det;

	for (int i = 0; i < 16; i++)
		elements[i] = inv[i] * det;
	return true;
}
inline Mat4x4 Mat4x4::operator*(const Mat4x4& rhs)
{
	Vec4f colums[4]
	{
		{ rhs[0][0],rhs[1][0],rhs[2][0],rhs[3][0] },
		{ rhs[0][1],rhs[1][1],rhs[2][1],rhs[3][1] },
		{ rhs[0][2],rhs[1][2],rhs[2][2],rhs[3][2] },
		{ rhs[0][3],rhs[1][3],rhs[2][3],rhs[3][3] }
	};
	Mat4x4 result{};
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			result.elements[y * 4 + x] = rows[y].dot(colums[x]);
		}
	}
	return result;
}
inline Vec4f Mat4x4::operator*(const Vec4f& rhs)
{
	Vec4f result{ rhs.dot(m_X),rhs.dot(m_Y),rhs.dot(m_Z),rhs.dot(m_W) };
	return result;
}

inline Mat4x4& Mat4x4::operator=(const Mat4x4& rhs)
{
	m_X = rhs.m_X;
	m_Y = rhs.m_Y;
	m_Z = rhs.m_Z;
	m_W = rhs.m_W;
	return *this;
}
inline  const Vec4f& Mat4x4::operator[](const int index) const
{
	assert(index <= 3 && index >= 0);
	return rows[index];
}
inline Mat4x4 Mat4x4::rotateX(float degrees)
{
	Mat4x4 rotateXMat
	{
		Vec4f{1.f,0.f,0.f,0.f},
		Vec4f{0.f,cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f},
		Vec4f{0.f,sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};

	*this = *this * rotateXMat;
	return *this;
}

inline Mat4x4 Mat4x4::rotateY(float degrees)
{
	Mat4x4 rotateYMat
	{
		  Vec4f{cosf(degreesToRads(degrees)),  0.f,	sinf(degreesToRads(degrees)),		0.f},
		Vec4f{0.f,								1.f,	0.f,									0.f},
		Vec4f{-sinf(degreesToRads(degrees)), 0.f,	cosf(degreesToRads(degrees)),		0.f},
		Vec4f{0.f,								0.f,	0.f,									1.f}
	};
	*this = *this * rotateYMat;
	return *this;
}

inline Mat4x4 Mat4x4::rotateZ(float degrees)
{
	Mat4x4 rotateZMat
	{
		Vec4f{cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f,0.f},
		Vec4f{sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f,0.f},
		Vec4f{0.f,0.f,1.f,0.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
	*this = *this * rotateZMat;
	return *this;
}

inline Mat4x4& Mat4x4::scaleUniform(float scale)
{
	rows[0][0] *= scale;
	rows[1][1] *= scale;
	rows[2][2] *= scale;
	return *this;
}

inline Mat4x4& Mat4x4::scale(const Vec3f& scale)
{
	rows[0][0] *= scale.m_x;
	rows[1][1] *= scale.m_y;
	rows[2][2] *= scale.m_z;
	return *this;
}



inline Mat4x4& Mat4x4::operator*=(const Mat4x4& rhs)
{
	Vec4f colums[4]
	{
		{ rhs[0][0],rhs[1][0],rhs[2][0],rhs[3][0] },
		{ rhs[0][1],rhs[1][1],rhs[2][1],rhs[3][1] },
		{ rhs[0][2],rhs[1][2],rhs[2][2],rhs[3][2] },
		{ rhs[0][3],rhs[1][3],rhs[2][3],rhs[3][3] }
	};
	Mat4x4 result{};
	for (int y = 0; y < 4; ++y)
	{
		for (int x = 0; x < 4; ++x)
		{
			result.elements[y * 4 + x] = rows[y].dot(colums[x]);
		}
	}
	*this = result;
	return *this;
}

inline Vec3f Mat4x4::getTranslation()
{
	return { rows[0][3],rows[1][3],rows[2][3] };
}
inline void Mat4x4::setTranslation(const Vec3f& pos)
{
	rows[0][3] = pos.m_x;
	rows[1][3] = pos.m_y;
	rows[2][3] = pos.m_z;
}
inline   Mat4x4 Mat4x4::operator+(const Mat4x4& rhs) const
{
	return Mat4x4(rows[0] + rhs[0], rows[1] + rhs[1], rows[2] + rhs[2], rows[3] + rhs[3]);
}

inline Mat4x4 Mat4x4::operator-(const Mat4x4& rhs) const
{
	return Mat4x4(rows[0] - rhs[0], rows[1] - rhs[1], rows[2] - rhs[2], rows[3] - rhs[3]);
}

inline Mat4x4& Mat4x4::operator-=(const Mat4x4& rhs)
{
	rows[0] -= rhs[0];
	rows[1] -= rhs[1];
	rows[2] -= rhs[2];
	rows[3] -= rhs[3];
	return *this;
}


inline bool operator==(const Mat4x4& lsh, const Mat4x4& rhs)
{
	bool equal{ true };
	for (int i = 0; i < 16; ++i)
	{
		if (!((lsh.elements[i] - rhs.elements[i]) >= -epsilon && (lsh.elements[i] - rhs.elements[i]) <= epsilon))
			equal = false;
	}
	return  equal;
}

inline bool operator!=(const Mat4x4& lsh, const Mat4x4& rhs)
{
	return !(lsh == rhs);
}
inline   Mat4x4& Mat4x4::operator+=(const Mat4x4& rhs)
{
	rows[0] += rhs[0];
	rows[1] += rhs[1];
	rows[2] += rhs[2];
	rows[3] += rhs[3];
	return *this;
}
#pragma warning(pop)
