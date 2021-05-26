#pragma once
#include "vec3f.h"
#include "Mat4x4.h"
class Transform
{
public:
	Transform() = default;
	~Transform() = default;

	 Mat4x4& getLocalMatrix();
	 Mat4x4& getInverseMatrix();

	void scale(const Vec3f& scale);
	void scaleUniform(float scale);
	void setLocation(const Vec3f& location);
	void rotateX(const float degrees);
	void rotateY(const float degrees);
	void rotateZ(const float degrees);
private:
	void updateMatrix();
	Mat4x4 finalRotMatrix();
	Mat4x4 finalScaleMatrix();
	Mat4x4 finalTranslationMatrix();
	Vec3f _scale{1.f,1.f,1.f};
	Vec3f _rotation{};
	Vec3f _position{};

	Mat4x4 _local{};
	Mat4x4 _inverse{};
	
};

