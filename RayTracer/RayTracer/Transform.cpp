#include "Transform.h"

 Mat4x4& Transform::getLocalMatrix()
{
	return  _local;
}
 Mat4x4& Transform::getInverseMatrix()
{
	return  _inverse;
}

void Transform::scale(const Vec3f& scale)
{
	_scale *= scale;
	updateMatrix();
}

void Transform::scaleUniform(float scale)
{

	_scale *= scale;
	updateMatrix();
}

void Transform::setLocation(const Vec3f& location)
{
	_position = location;
	updateMatrix();
}

void Transform::rotateX(const float degrees)
{
	_rotation.m_x += degrees;
	updateMatrix();
}

void Transform::rotateY(const float degrees)
{
	_rotation.m_y += degrees;
	updateMatrix();
}

void Transform::rotateZ(const float degrees)
{
	_rotation.m_z += degrees;
	updateMatrix();
}

void Transform::updateMatrix()
{
	Mat4x4 result{ finalTranslationMatrix() * finalRotMatrix() * finalScaleMatrix() };
	_local = result;
	_inverse = result;
	_inverse.inverse();
}

Mat4x4 Transform::finalRotMatrix()
{
	Mat4x4 result{};
	result.rotateX(_rotation.m_x);
	result.rotateY(_rotation.m_y);
	result.rotateZ(_rotation.m_z);
	return result;
}

Mat4x4 Transform::finalScaleMatrix()
{
	Mat4x4 result{};
	result.scale(_scale);
	return result;
}

Mat4x4 Transform::finalTranslationMatrix()
{
	Mat4x4 result{};
	result.setTranslation(_position);
	return result;
}
