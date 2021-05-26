#pragma once
#include "Scene.h"
#include "Utility.h"

class testScene : public Scene
{
public:
	testScene();
	~testScene();
	void tick(float deltaTime) override;
	void endTick(float deltaTime) override;
private:
	float angle{};
	float angle2{ (2 * M_PI) / 360.f };

	enum  TexturesNames
	{
		t_crate = 0,
		t_crateNormal,
		t_whooleNormal,
		t_realisticGlobe,
		t_realisticGlobeNormal,
		t_realisticMars,
		t_realisticMarsNormal,
		t_realisticMoon,
		t_realisticMoonNormal,
		t_testNormal,
		t_MAX
	};

	enum MaterialNames
	{
		m_globe = 0,
		m_mars,
		m_moon,
		m_testNormalMaterial,
		m_crate,
		m_MAX
	};
	Image textures[static_cast<int>(TexturesNames::t_MAX)]
	{
			Image{ "assets/crate.png",480,480,4 },
		 Image{ "assets/crateNormal.png",480,480,4 },
		 Image{ "assets/whoolNormal.jpg",800,800,3 },
		 Image{ "assets/globe.jpg",1024,512,3 },
		 Image{ "assets/globeNormal.jpg",1024,512,3 },
		 Image{ "assets/mars.jpg",1024,512,3 },
		 Image{ "assets/marsNormal.jpg",1024,512,3 },
		 Image{ "assets/moon.jpg",1024,512,3 },
		 Image{ "assets/moonNormal.jpg",1024,512,3 },
		 Image{ "assets/testNormal.png",640,640,4 }
	};
	Material materials[static_cast<int>(MaterialNames::m_MAX)]
	{
		{ 0.f,0.18f,0.f,&textures[TexturesNames::t_realisticGlobe],&textures[TexturesNames::t_realisticGlobeNormal] },
		{ 0.f,0.18f,0.f,&textures[TexturesNames::t_realisticMars],&textures[TexturesNames::t_realisticMarsNormal] },
		{ 0.f,0.18f,0.f,&textures[TexturesNames::t_realisticMoon],&textures[TexturesNames::t_realisticMoonNormal] },
		{ 0.f,0.18f,0.f,nullptr,&textures[TexturesNames::t_testNormal] },
		{ 0,0.18f,0,&textures[TexturesNames::t_crate],&textures[TexturesNames::t_crateNormal] }
	};
	
	
};
