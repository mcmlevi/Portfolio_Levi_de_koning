#include "shape.h"
#include "Utility.h"

Shape::Shape(Vec3f orgin, Color color, Material mat) :m_orgin(orgin), m_color(color),m_mat(mat)
{
}

void Shape::rotateX(const float& degrees)
{
	//Mat4x4 rotateXMat
	//{
	//	Vec4f{1.f,0.f,0.f,0.f},
	//	Vec4f{0.f,cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f},
	//	Vec4f{0.f,sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f},
	//	Vec4f{0.f,0.f,0.f,1.f}
	//};

	////rotation = (rotation * rotateXMat );
	////updateMatrix();
	//updateLocalMatrix(localMatrix * rotateXMat);
	transform.rotateX(degrees);
}

void Shape::rotateY(const float& degrees)
{
	//Mat4x4 rotateYMat
	//{
	//	  Vec4f{cosf(degreesToRads(degrees)),  0.f,	sinf(degreesToRads(degrees)),		0.f},
	//	Vec4f{0.f,								1.f,	0.f,									0.f},
	//	Vec4f{-sinf(degreesToRads(degrees)), 0.f,	cosf(degreesToRads(degrees)),		0.f},
	//	Vec4f{0.f,								0.f,	0.f,									1.f}
	//};
	////rotation = rotation * rotateYMat;
	////updateMatrix();
	//updateLocalMatrix(localMatrix * rotateYMat);
	transform.rotateY(degrees);
}

void Shape::rotateZ(const float& degrees)
{
	//Mat4x4 rotateZMat
	//{
	//	Vec4f{cosf(degreesToRads(degrees)),-sinf(degreesToRads(degrees)),0.f,0.f},
	//	Vec4f{sinf(degreesToRads(degrees)),cosf(degreesToRads(degrees)),0.f,0.f},
	//	Vec4f{0.f,0.f,1.f,0.f},
	//	Vec4f{0.f,0.f,0.f,1.f}
	//};
	////rotation = rotation * rotateZMat;
	////updateMatrix();
	//updateLocalMatrix(localMatrix * rotateZMat);s
	transform.rotateZ(degrees);
}

Color Shape::getUVColor(float u, float v, const Image* image)
{
	u = clamp(u, 0.f, 1.f);
	v = clamp(v, 0.f, 1.f);
	int x{ static_cast<int>( u * image->width) };
	x = clamp(x, 0, image->width);
	int y{ static_cast<int>(v * image->height-1) };
	y = clamp(y, 0, image->height);
	unsigned char* colorPtr{ &image->image[y * image->width * 3 + x * 3] };
	Color result{(*(colorPtr)), (*(colorPtr +1 )),(*(colorPtr +2)) };
	return result;
}

Vec3f Shape::calculateBumpNormal(float u, float v, Vec3f& worldNormal)
{
	Vec3f t{ worldNormal.cross({0.f,1.f,0.f}) };
	if (t.lenght() < epsilon)
	{
		t = worldNormal.cross({ 0.f,0.f,1.f });
	}
	t.normalize();
	Vec3f b{ worldNormal.cross(t).normalize() };

	Color mapColor{ getUVColor(u,v,m_mat.bumpMap) };
	Vec3f mapN{ mapColor.R,mapColor.G,mapColor.B };
	mapN.normalize();
	mapN = (mapN * 2) - Vec3f{ 1.f, 1.f, 1.f };
	mapN.normalize();
	Mat4x4 tbn
	{
		Vec4f{t,0.f},
		Vec4f{b,0.f},
		Vec4f{worldNormal,0.f},
	{0.f,0.f,0.f,1.f}
	};
	return (tbn * Vec4f{ mapN,0.f }).m_XYZ.normalize();
}




