#include "pch.h"
#include "TBRGame.h"
#include <iostream>
#include "IMapParser.h"
#include "JsonParser.h"
#include "LevelManager.h"
#include "TowerSystem.h"
#include <Core/MessageLogger.h>
#include <Core/ResourceManager.h>
#include <Core/WorldManager.h>
#include <Components/TransformComponent.h>
#include <Components/ModelComponent.h>
#include <Components/CameraComponent.h>
#include <Resources/Shader.h>

#include <glm/gtc/random.hpp>

#include "CameraSystem.h"
#include "EnemySystem.h"
#include "PlayerSystem.h"
#include "ProjectileSystem.h"
#include "WaveSystem.h"
#include "Core/Audio/AudioManager.h"
using namespace Th::Components;

void TBRGame::Initialize()
{
	//Initialize the core.
	Th::Core::Initialize();

#if PLATFORM_PS4
	Th::Core::MainLoop();
#endif

	CameraSystem camSys;
	//Ease for syntax, do *never* store as member variables.
	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

	TransformComponent transform = glm::vec3(35,50,20);//(glm::vec3(-20.0f, 0, -20.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	entt::entity cameraEntity = worldManager.CreateEntity("Camera", transform);

	LevelManager m_LevelManager;

	worldManager.GetEntityRegister().emplace<CameraComponent>(cameraEntity);
	worldManager.GetEntityRegister().get<CameraComponent>(cameraEntity).Initialize(70, 1600, 900);
	m_RenderSystem.SetActiveCamera(cameraEntity);

	//Loading of model and shader.
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Tower/weapon_blaster.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyGreen.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyGreenWG.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyPurple.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyPurpleWG.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyRed.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyRedWG.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyYellow.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Enemy/EnemyYellowWG.obj", Th::EModelFileType::OBJ);

	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Projectile/BulletBlack.obj", Th::EModelFileType::OBJ);
	resourceManager.LoadModel(m_RenderSystem.GetResourceLoader(), "[models]/Projectile/BulletYellow.obj", Th::EModelFileType::OBJ);

	resourceManager.LoadShader(m_RenderSystem.GetResourceLoader(), "[shaders]/TriangleShader.vert.cso", "[shaders]/TriangleShader.frag.cso");


	m_LevelManager.SetTileModelShader(&m_RenderSystem);
	//m_LevelManager.LoadLevel("res/levels/FinalTestLevel2.json");
	m_LevelManager.LoadLevel("res/levels/SmallerLevel.json");
	PlayerSystem playersys;
	ProjectileSystem projectileSystem;
	TowerSystem towersys;
	towersys.Initialize(&m_LevelManager, &playersys);
	towersys.SetTowerModels(&m_RenderSystem);
	EnemySystem enemySys(&m_LevelManager);
	WaveSystem waveSys(enemySys, m_LevelManager);
	enemySys.Initialize(&playersys, waveSys);

	Th::AudioManager& audioManager = Th::AudioManager::GetInstance();
	audioManager.LoadBank("res/audio/Events.bank");
	audioManager.LoadBank("res/audio/Events.strings.bank");
	audioManager.PlayEvent("event:/FX/hit", glm::vec3(0,0,0), 1.0f);
	//Start the mainloop after creation is done - since the core's mainloop is a whileloop that is not really open for the consumer right now.
	//For updating systems make sure to use the system wrapper from Mike.
	Th::Core::MainLoop();
}

void TBRGame::Shutdown()
{

}

Th::Core* Th::CreateGame()
{
	return new TBRGame();
}