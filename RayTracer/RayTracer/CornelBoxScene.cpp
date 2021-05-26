#include "CornelBoxScene.h"
#include "shapes.h"
#include "Light.h"

CornelBoxScene::CornelBoxScene() :Scene(8, 1)
{
	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,2.f},
	Vec4f{0.f,0.f,1.f,4.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};
	lights[0] = new Light{ {0.f,4.f,0.f}, {1.f,1.f,1.f},5.f };

	// Left wall
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,0.f,0.f }, {}));
	// Right wall
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 0.f,1.f,0.f }, {}));
	// Roof
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, {}));
	// floor
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, {}));
	// Back wall
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, {}));
	// Rotated box
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,0.f,0.f }, {}));
	// Sphere
	shapes.push_back(new Sphere{ {0.f,0.f,0.f},1.f,{1.f,1.f,1.f},{0.8f,0.18f,0.f} });
	shapes.push_back(new Sphere{ {0.f,0.f,0.f},1.f,{0.1f,0.1f,0.1f},{0.0f,0.18f,1.5f} });

	// left wall
	shapes[0]->transform.scale({ 1.f,5.f,10.f });
	shapes[0]->transform.setLocation({ -5.f,2.5f,0.f });
	//right wall
	shapes[1]->transform.scale({ 1.f,5.f,10.f });
	shapes[1]->transform.setLocation({ 5.f,2.5f,0.f });
	//Roof
	shapes[2]->transform.scale({ 10.f,1.f,10.f });
	shapes[2]->transform.setLocation({ 0.f,5.0f,0.f });

	// floor
	shapes[3]->transform.scale({ 10.f,1.f,10.f });
	// wall
	shapes[4]->transform.scale({ 10.f,5.f,1.f });
	shapes[4]->transform.setLocation({ 0.f,2.5f,-2.5f });

	//Box
	shapes[5]->transform.scale({ 1.f,2.f,1.f });
	shapes[5]->transform.setLocation({ -2.f,1.5f,-1.f });
	shapes[5]->rotateY(45);

	// refelctive  Sphere
	shapes[6]->transform.setLocation({ 1.5f,1.5f,0.f });

	// refractive Sphere
	shapes[7]->transform.setLocation({ 0.f,1.5f,1.5f });

	//numOfShapes = 7;
}

void CornelBoxScene::tick(float deltaTime)
{
	static_cast<void>(deltaTime);
}

void CornelBoxScene::endTick(float deltaTime)
{
	static_cast<void>(deltaTime);
}