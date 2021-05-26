#include "pch.h"
#include "EnemySystem.h"
#include "EnemyComponent.h"
#include "Core/ResourceManager.h"
#include "Core/WorldManager.h"
#include "TowerComponent.h"
#include "PlayerSystem.h"
#include "WaveSystem.h"
#include "Core/MessageLogger.h"

struct TowerComponent;

EnemySystem::EnemySystem(LevelManager* levelManager)
{
	Register();
	m_LevelManager = levelManager;
	m_StartTimer = 10;
}

void EnemySystem::Initialize(PlayerSystem* sys, WaveSystem& waveSys)
{	
	m_System = sys;
	m_WaveSystem = &waveSys;
}

void EnemySystem::CreateEnemy(float speed, int health, glm::vec3 position, EnemyType enemytype, int gold)
{
	Th::ResourceManager& resourceManager = Th::ResourceManager::GetInstance();
	Th::WorldManager& worldManager = Th::WorldManager::GetSingleton();

	//create a transform component
	Th::TransformComponent transform(position, entt::null, glm::vec3(0), glm::vec3(0.5f));

	auto ent = worldManager.CreateEntity(std::string("Enemy" + std::to_string(m_Enemies.size())), transform);

	//load in the model used for the tower
	Th::ModelComponent* model;
	std::string path = "";
	switch (enemytype)
	{
	case EnemyType::Red:
		path = "[models]/Enemy/EnemyRed.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::RedWG:
		path = "[models]/Enemy/EnemyRedWG.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::Green:
		path = "[models]/Enemy/EnemyGreen.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::GreenWG:
		path = "[models]/Enemy/EnemyGreenWG.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::Yellow:
		path = "[models]/Enemy/EnemyYellow.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::YellowWG:
		path = "[models]/Enemy/EnemyYellowWG.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::Purple:
		path = "[models]/Enemy/EnemyPurple.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	case EnemyType::PurpleWG:
		path = "[models]/Enemy/EnemyPurpleWG.obj";
		model = resourceManager.GetModel(path);
		for (int i = 0; i < resourceManager.GetModel(path)->GetMeshCount(); i++)
		{
			(*resourceManager.GetModel(path))[i].RenderShader = *resourceManager.GetShader("[shaders]/TriangleShader.vert.cso");
		}
		break;
	default:;
	}
	worldManager.GetEntityRegister().emplace<Th::ModelComponent>(ent, *model);

	//create the actual tower
	EnemyComponent EnemyComp;
	EnemyComp.Initialize(speed, health, gold);

	//add the tower to the entity
	worldManager.GetEntityRegister().emplace<EnemyComponent>(ent, EnemyComp);
	EnemyComponent& enemy = GetEntityRegister().get<EnemyComponent>(ent);
	enemy.m_ActivePath = &m_LevelManager->m_GeneratedPath;
	m_Enemies.push_back(ent);
	m_WaveSystem->AddToWave(ent);
}

void EnemySystem::OnUpdate(float dt)
{
	if (m_WaveSystem != nullptr)
	{
		if (!m_Initialized)
		{
			m_Time += dt;
			if (m_Time >= m_StartTimer)
			{
				m_WaveSystem->CreateWaves(20, 1, 2);
				m_Time = 0;
				m_Initialized = true;
			}
		}
	}
	else
	{
		LOGERROR("Wavesystem missing");
	}
	
	for (auto entity : GetEntityRegister().view<EnemyComponent>())
	{
		EnemyComponent& enemy = GetEntityRegister().get<EnemyComponent>(entity);
		Th::TransformComponent& transform = GetEntityRegister().get<Th::TransformComponent>(entity);
		glm::vec3 pos = transform.GetPosition();
		

		Th::TransformComponent& TargetTransform = GetEntityRegister().get<Th::TransformComponent>(m_LevelManager->m_GeneratedPath[enemy.m_ActiveTargetTileIndex]->m_TileEntity);
		glm::vec3 targetPos = TargetTransform.GetPosition();
		targetPos.y += 0.6f;
		
		if (enemy.MoveToTarget(targetPos, &transform, dt)) {
			if (enemy.m_ActiveTargetTileIndex < m_LevelManager->m_GeneratedPath.size() - 1) {
				enemy.m_ActiveTargetTileIndex++;
			}
			else if(enemy.m_ActiveTargetTileIndex >= m_LevelManager->m_GeneratedPath.size() - 1) {
				//reached end
				std::cout << "reached end" << std::endl;

				//Add damage to player tower
				for (auto ent : GetEntityRegister().view<TowerComponent>())
				{
					TowerComponent& tower = GetEntityRegister().get<TowerComponent>(ent);
					tower.RemoveEnemyFromNearbyList(entity);
				}
				m_WaveSystem->RemoveFromList(entity);
				DeleteEntity(entity);
			}

		}
		if (enemy.GetHealth() <= 0)
		{
			m_System->AddGold(enemy.GetWorth());
			for (auto ent : GetEntityRegister().view<TowerComponent>())
			{
				TowerComponent& tower = GetEntityRegister().get<TowerComponent>(ent);
				tower.RemoveEnemyFromNearbyList(entity);
			}
			m_WaveSystem->RemoveFromList(entity);
			DeleteEntity(entity);
		}
	}
}
