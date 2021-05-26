#include "Mat4x4.h"
#include "Utility.h"



  Vec4f& Mat4x4::operator[](const int index)
{
	assert(index <= 3 && index >= 0);
	return rows[index];
}



  Mat4x4 Mat4x4::operator*(const float& scalar) const
{
	return Mat4x4(rows[0] * scalar, rows[1] * scalar, rows[2] * scalar, rows[3] * scalar);
}














 float Mat4x4::determinant()
{

	return 0.0f;
}

 void Mat4x4::transpose()
{
	Vec4f x{ (m_X.m_XYZ)[0],(m_Y.m_XYZ)[0],m_Z.m_XYZ[0],(m_W.m_XYZ)[0] };
	Vec4f y{ (m_X.m_XYZ)[1],(m_Y.m_XYZ)[1],m_Z.m_XYZ[1],(m_W.m_XYZ)[1] };
	Vec4f z{ (m_X.m_XYZ)[2],(m_Y.m_XYZ)[2],m_Z.m_XYZ[2],(m_W.m_XYZ)[2] };
	Vec4f w{ m_X.m_coordinates[3],m_Y.m_coordinates[3],m_Z.m_coordinates[3],m_W.m_coordinates[3] };
	m_X = x;
	m_Y = y;
	m_Z = z;
	m_W = w;
}







