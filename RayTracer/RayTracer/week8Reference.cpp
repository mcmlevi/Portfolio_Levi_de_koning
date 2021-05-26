#include "week8Reference.h"
#include "shapes.h"
#include "Light.h"
week8Reference::week8Reference():Scene(32768,1)
{

	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,1.f},
	Vec4f{0.f,0.f,1.f,3.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};

	shapes.push_back(new Plane({ 0.f,0.f,0.f }, { 0.5f,0.5f,0.5f }, {}, { 0.5f,0.5f,0.5f }));
	shapes.push_back(new Sphere({}, 1.f, { 1.f,1.f,1.f }, {}));


	shapes[1]->transform.setLocation({ 0.f,1.f,0.f });
	shapes[1]->m_mat.reflectiveIndex = 0.8f;
	
	lights[0] = new Light{ { 0.f,20.f,0.f } ,{1.f,1.f,1.f},100.f };
	for (int i = 2; i < numOfShapes; ++i)
	{
		float R{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f))) };
		float G{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f))) };
		float B{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f))) };


		shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { R,G,B }, m_crate))  ;
		float x{};
		float y{ 1.f };
		float z{};
		bool mayNotExit{ true };

		do
		{
			x = { -50.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.f - -50.f))) };
			z = { -50.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.f - -50.f))) };

			if (x * x + z * z > 25.f)
				mayNotExit = false;
			
		}
		while (mayNotExit);
		
		

		float rotX{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ( 360.f - 1.f))) };
		float rotY{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ( 360.f - 1.f))) };
		//float rotZ{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ( 360.f - 1.f))) };

		float scaleX{ 1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.f - 1.f))) };
		float scaleY{ 1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.f - 1.f))) };
		float scaleZ{ 1.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (3.f - 1.f))) };

		//randShere.rotateZ(rotZ);
		shapes[i]->transform.scale({ scaleX,scaleY,scaleZ });
		shapes[i]->transform.setLocation({ x,y,z });
		shapes[i]->transform.rotateX(rotX);
		shapes[i]->transform.rotateY(rotY);
	}

	
}

void week8Reference::tick(float deltaTime)
{
	static_cast<void>(deltaTime);
}

void week8Reference::endTick(float deltaTime)
{
	static_cast<void>(deltaTime);
}
