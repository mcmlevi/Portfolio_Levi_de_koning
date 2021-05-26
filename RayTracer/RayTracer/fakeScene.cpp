#include "fakeScene.h"
#include "shapes.h"
#include "Light.h"
fakeScene::fakeScene() :Scene(1, 1)
{
	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,2.f},
	Vec4f{0.f,0.f,1.f,10.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};
	shapes.push_back( new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, fakeMat));

	//shapes[1]->updateLocalMatrix(longBoi);
	shapes[0]->transform.scale({9.74f,5.97f,10.f});
	
	lights[0] = new Light{ { 0.0f,8.f,10.f } ,{1.f,1.f,1.f},50.f };
}

void fakeScene::tick(float deltaTime)
{
	static_cast<void>(deltaTime);
}

void fakeScene::endTick(float deltaTime)
{
	static_cast<void>(deltaTime);
}
