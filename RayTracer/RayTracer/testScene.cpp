#include "testScene.h"
#include "Vec4f.h"
#include "Mat4x4.h"
#include "shapes.h"
#include "Light.h"

#include "Mesh.h"
testScene::testScene() :Scene(9, 1)
{
	cameraLocation =
	{
	Vec4f{1.f,0.f,0.f,0.f},
	Vec4f{0.f,1.f,0.f,2.f},
	Vec4f{0.f,0.f,1.f,10.f},
	Vec4f{0.f,0.f,0.f,1.f}
	};

	shapes.push_back(new Plane({ 0.f,0.f,0.f }, { 1.f,1.f,1.0f, }, {}));
	shapes.push_back(new Sphere({}, 1.f, { 1.f,1.f,1.f }, {}));
	shapes.push_back(new Sphere({}, 0.75f, { 0.f,1.f,0.f }, {}));
	shapes.push_back(new Sphere({}, 0.75f, { 1.f,0.f,0.f }, {}));
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 0.2f,0.f,0.f }, {}));
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, materials[m_crate]));
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, materials[m_crate]));

	shapes.push_back(new Sphere({}, 1.f, { 1.f,0.f,0.f }, materials[m_globe]));
	shapes.push_back(new OBox({ -0.5f,-0.5f,0.5f }, { 0.5f,0.5f,-0.5f }, { 1.f,1.f,1.f }, materials[m_testNormalMaterial]));

	shapes[1]->m_mat.reflectiveIndex = 0.3f;
	shapes[1]->m_mat.refractionIndex = 1.3f;
	shapes[4]->m_mat.reflectiveIndex = 0.3f;
	shapes[4]->m_mat.refractionIndex = 1.3f;





	Mat4x4 testCubeMat
	{
		 Vec4f{1.f,0.f,0.f,-6.f},
		Vec4f{0.f,1.f,0.f,2.f},
		Vec4f{0.f,0.f,1.f,6.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};
	//shapes[1]->updateLocalMatrix(longBoi);
	shapes[1]->transform.setLocation({ 0.f,2.f,-4.f });
	shapes[1]->transform.scaleUniform(1.5f);

	shapes[2]->transform.setLocation({ -1.25f,2.f,-6.f });
	shapes[2]->transform.scale({ 2.f,1.f,1.f });
	shapes[2]->m_mat.refractionIndex = 1.55f;

	shapes[3]->transform.setLocation({ 1.25f,2.f,-6.f });

	
	shapes[4]->transform.setLocation({0.f,2.f,0.f});
	shapes[4]->transform.scale({ 2.f,3.f,2.f });
	
	
	shapes[5]->transform.setLocation({4.f,2.f,-13.f});
	shapes[5]->transform.scaleUniform(2.f);
	
	shapes[6]->transform.setLocation({ 8.f,2.f,-13.f });
	shapes[6]->transform.scaleUniform(2.f);
	
	shapes[7]->transform.setLocation({ 6.f,2.f,-6.f });
	shapes[8]->transform.setLocation({ -6.f,2.f,6.f });
	shapes[8]->rotateY(45);


	lights[0] = new Light{ { 0.5f,8.5,-5.f } ,{1.f,1.f,1.f},25.f };
	//for (int i = 9; i < 1024; ++i)
	// {
	//	 float R{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f)))  };
	//	 float G{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f)))  };
	//	 float B{ static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.f)))  };

	//
	//	 shapes[i] = new Sphere({}, 1.f, { R,G,B }, {});
	//	 float x{ -50.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.f - -50.f))) };
	//	 float y{1.f};
	//	 float z{ -50.f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (50.f - -50.f))) };
	//	 Mat4x4 randShere
	//	 {
	//		  Vec4f{1.f,0.f,0.f,x},
	//		 Vec4f{0.f,1.f,0.f,y},
	//		 Vec4f{0.f,0.f,1.f,z},
	//		 Vec4f{0.f,0.f,0.f,1.f}
	//	 };
	//	 shapes[i]->updateLocalMatrix(randShere);
	// }
	//
}

testScene::~testScene()
{
}

void testScene::tick(float deltaTime)
{
	lights[0]->m_lightLocation = Vec3f{ sinf(angle) * 5, 6.f,-5.f };


	Mat4x4 longBoi3
	{
		 Vec4f{1.f,0.f,0.f,1.25f},
		Vec4f{0.f,1.f,0.f,sinf(angle + M_PI * 0.15f) + 2},
		Vec4f{0.f,0.f,1.f,-6.f},
		Vec4f{0.f,0.f,0.f,1.f}
	};

	
	shapes[2]->transform.setLocation({-1.25f,sinf(angle + M_PI * 0.5f) + 2.f,-6.f });
	shapes[3]->transform.setLocation({ 1.f,sinf(angle + M_PI * 0.15f) + 2.f,-6.f });

	shapes[4]->rotateX(angle2 * (180.f / M_PI) * deltaTime * 0.05f);
	shapes[5]->rotateY(angle2 * (180.f / M_PI) * deltaTime * 0.05f);
	shapes[6]->rotateX(angle2 * (180.f / M_PI) * deltaTime * 0.05f);
	shapes[6]->rotateY(angle2 * (180.f / M_PI) * deltaTime * 0.05f);
	shapes[7]->rotateY(angle2 * (180.f / M_PI) * deltaTime * 0.05f);
}

void testScene::endTick(float deltaTime)
{
	angle += ((2 * M_PI) / 360) * deltaTime * 0.025f;
}