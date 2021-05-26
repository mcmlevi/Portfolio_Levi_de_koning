#include "meshTestScene.h"
#include "Light.h"
#include "Triangle.h"
#include "Mesh.h"
#include "Plane.h"

meshTestScene::meshTestScene() :Scene(2, 1)
{
	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,-9.5f},
	Vec4f{0.f,1.f,0.f,7.f},
	Vec4f{0.f,0.f,1.f,-3.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};
	cameraLocation.rotateX(-30);
	cameraLocation.rotateY(-40);
	cameraLocation.rotateZ(-30);
	cameraLocation.rotateY(-20);
	shapes.push_back(new Plane({ 0.f,-2.f,0.f }, { 1.f,1.f,1.0f, }, {}));
	Color white{ 1.f,1.f,1.f };
	Color black{ 0.f,0.f,0.f };
	Color red{ 1.f,0.f,0.f };
	Color mild{ 0.1f,0.1f,0.1f };
	//shapes[0] = new Triangle{ {0.f,0.f,0.f},{1.f,0.f,0.f},{0.5f,-0.5f,1.f},{1.f,1.f,1.f},{} };
	//shapes.push_back( new Mesh("assets/teddy.obj",TriangleType::SIMPLE,red,refraction)) ;
	//shapes.push_back(new Mesh("assets/sphere.obj", TriangleType::COMPLEX, red, refraction));
	//shapes.push_back(new Mesh("assets/highResSphere.obj", TriangleType::COMPLEX, red, refraction));
	//shapes[1] = new Mesh("assets/smallTest.obj",TriangleType::COMPLEX);
	//shapes.push_back(new Mesh("assets/Dragon.obj", TriangleType::NO_TEXTURE,white,reflectiveDragon));
	//shapes.push_back(new Mesh("assets\\lowPolyDragon.obj", TriangleType::NO_TEXTURE,black,refraction));
	//shapes.push_back(new Mesh("assets/bunny.", TriangleType::NO_TEXTURE,white,OpaqueDragon));
	//Mat4x4 teddyMat =
	//{
	//Vec4f{0.1f,0.f,0.f,0.f},
	//Vec4f{0.f,0.1f,0.f,0.f},
	//Vec4f{0.f,0.f,0.1f,0.f},
	//Vec4f{0.f,0.f,0.f,1.f}
	//};
	//teddyMat.scaleUniform(10.f);
	//shapes[1]->updateLocalMatrix(teddyMat);
	//shapes[1]->rotateY(45);

	//shapes.push_back(new Mesh{ "assets/DragonCute.obj", TriangleType::COMPLEX_QUAD,white, OpaqueDragon });
	shapes.push_back(new Mesh{ "assets/ferari.obj",TriangleType::NO_TEXTURE,mild,OpaqueDragon });

	//shapes[1]->transform.scaleUniform(0.75f);
	//shapes[1]->transform.scaleUniform(0.1f);
	//shapes[1]->transform.setLocation({ 0,3.f,-2.f });
	//shapes[1]->transform.rotateX(-90);

	//shapes[1]->m_mat.refractionIndex = 1.3f;

	lights[0] = new Light{ { 0.0f,40.f,20.f } ,{1.f,1.f,1.f},200.f };
	//lights[1] = new Light{ { 0.0f,20.f,20.f } ,{1.f,1.f,1.f},60.f };
	Mesh* mesh1{ static_cast<Mesh*>(shapes[1]) };
	for (unsigned i = 0; i < mesh1->triangles.size(); ++i)
	{
		shapes.push_back(mesh1->triangles[i]);
		shapes.back()->transform.rotateX(-90.f);
		shapes.back()->transform.scaleUniform(1.f);
		++numOfShapes;
	}
}

meshTestScene::~meshTestScene()
{
	numOfShapes = 2;
}


void meshTestScene::tick(float deltaTime)
{
	static_cast<void>(deltaTime);
}

void meshTestScene::endTick(float deltaTime)
{
	static_cast<void>(deltaTime);
}