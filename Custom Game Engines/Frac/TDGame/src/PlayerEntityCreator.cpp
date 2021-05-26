#include "pch.h"
#include "PlayerEntityCreator.h"

PlayerEntityCreator::PlayerEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* connector) : IEntityCreator(sceneManager, tileManager) {
	m_world = &connector->GetEngine().GetWorld();
}

PlayerEntityCreator::~PlayerEntityCreator() {

}

void PlayerEntityCreator::CreatePlayerBase(int id, glm::vec3 position, glm::vec3 rotation, float coreHealth, float coreTargetData) {
	float entitySize = m_tileManager->m_tileMap->m_size - 0.25f;
	glm::vec2 basePos = { position.x, position.z };
	m_basePos = basePos;
	glm::vec3 WorldPos = m_tileManager->m_tileMap->GridPosToWorldPos(basePos);
	Frac::EntityHandle baseEntity = m_sceneManager->GetModel("Models/Models/Turrets/12.gltf", Frac::EModelFileType::GLTF);
	m_world->set<TOR::Transform>(baseEntity, { {WorldPos }, {glm::vec3(2,2,2)},{},{},{},{true},{true} });
	m_world->set<BoxComponent>(baseEntity, { {glm::vec4(1,0,1,1)}, true });
	m_world->set<GridPositionComponent>(baseEntity, {basePos});
	m_world->set<DirectionComponent>(baseEntity, {glm::vec3(0.001f,0,0)});
	m_world->add<BaseTag>(baseEntity);
	m_world->set<HealthComponent>(baseEntity, {});
	m_tileManager->SetTileOccupancy(basePos, true, baseEntity);
}

void PlayerEntityCreator::CreateTower(int id, glm::vec3 position, glm::vec3 rotation, float fireRate, float BulletSpeed) {
	float entitySize = m_tileManager->m_tileMap->m_size - 0.25f;
	glm::vec2 turretPos = { position.x, position.z };
	glm::vec3 WorldPos = m_tileManager->m_tileMap->GridPosToWorldPos(turretPos);

	Frac::EntityHandle baseEntity = m_world->makeEntity("Towers" + std::to_string(createdTower));
	std::cout << "Towers" + std::to_string(createdTower) << std::endl;
	//Frac::EntityHandle modelEntity = m_sceneManager->GetModel("Models/Models/Turrets/5.gltf", Frac::EModelFileType::GLTF);
	
	//m_world->AddInstancedPrefabToEntity(modelEntity, baseEntity);
	m_world->set<TOR::Transform>(baseEntity, { {WorldPos }, {glm::vec3(1,1,1)},{},{},{},{true},{true}});
	m_world->set<BoxComponent>(baseEntity, { {glm::vec4(0.53f, 0.78f, 1.f,1.f)}, true });
	m_world->set<GridPositionComponent>(baseEntity, { turretPos });
	m_world->set<DirectionComponent>(baseEntity, { glm::vec3(0.001f,0,0) });
	m_world->set<TurretComponent>(baseEntity, {});
	m_tileManager->SetTileOccupancy(turretPos, true, baseEntity);
	createdTower++;
	/*
	float entitySize = m_tileManager->m_tileMap->m_size - 0.25f;
	glm::vec2 turretPos = { position.x, position.z };
	glm::vec3 WorldPos = m_tileManager->m_tileMap->GridPosToWorldPos(turretPos);
	Frac::EntityHandle turretEntity = m_world->makeEntity<BoxComponent, GridPositionComponent, DirectionComponent, TurretComponent, TOR::Transform>
		("",
			{ {glm::vec4(0,1,1,1)}, true },
			{ {turretPos} },
			{ {glm::vec3(0.001f,0,0)} },
			{ {} },
			{ {WorldPos }, {glm::vec3(1,1,1)},{},{},{},{true},{true}});

			*/
	switch (id)
	{
	case 0: {
		//Creating tower models, TODO: Add the right models.
	}break;
	case 1: {
		//Creating tower models, TODO: Add the right models.
	}break;
	case 2: {
		//Creating tower models, TODO: Add the right models.
	}break;
	case 3: {
		//Creating tower models, TODO: Add the right models.
	}break;
	default:
		break;
	}
}

glm::vec2 PlayerEntityCreator::GetBasePos() const
{
	return m_basePos;
}