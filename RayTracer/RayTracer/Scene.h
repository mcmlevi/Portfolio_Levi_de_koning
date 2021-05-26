#pragma once
#include "vector.h"
class Shape;
struct Light;
#include "material.h"
#include "image.h"
#include "Mat4x4.h"
#include "Vec4f.h"
class Scene
{
public:
	Scene(int numShapes, int numLights);
	virtual ~Scene();
	int numOfShapes;
	int m_numberOfLights;
	ldk::Vector<Shape*> shapes;
	Light** lights;
	
	virtual void tick(float deltaTime) = 0;
	virtual void endTick(float deltaTime) = 0;

	Mat4x4 cameraLocation
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,1.f},
	Vec4f{0.f,0.f,1.f,0.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};
	
protected:

};

