#include "week5Scene.h"
#include "shapes.h"
#include "Light.h"
week5Scene::week5Scene(): Scene(3,1)
{
	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,2.f},
	Vec4f{0.f,0.f,1.f,3.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};

	
	shapes.push_back( new Plane({ 0.f,0.f,0.f }, { 1.f,0.f,0.0f, }, {},{1.f,1.f,0.f},{-2.f,-epsilon ,10.f },{7.f,epsilon,-10.f}));
	//shapes.push_back(new Plane({ 0.f,0.f,0.f }, { 1.f,0.f,0.0f, }, {}, { 1.f,1.f,0.f }));
	shapes.push_back( new Sphere({}, 2.f, { 0.1f,0.1f,0.1f }, {0.0f,0.18f,1.0f}));
	//shap.push_back( = new Sphere({}, 1.8f, { 1.f,1.f,1.f }, { 0.2f,0.18f,0.7f });
	shapes.push_back( new Sphere({}, 2.f, { 1.f,1.f,1.f }, {0.8f,0.18f,0.f}));

	
	
	


	

	
	shapes[1]->transform.setLocation({ 0.f,2.5f,-3.f });
	
	//shapes[1]->rotateZ(20.f);
	//shapes[2]->updateLocalMatrix(refraction);
	shapes[2]->transform.setLocation({3.f,2.f,-6.f});
	lights[0] = new Light{ {0.f,10.f,0.f},{1.f,1.f,1.f},10.f };
}

void week5Scene::tick(float deltaTime)
{
	static_cast<void>(deltaTime);
}

void week5Scene::endTick(float deltaTime)
{
	static_cast<void>(deltaTime);
}
