#include "pch.h"
#include "EnemyEntityCreator.h"

EnemyEntityCreator::EnemyEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* connector) : IEntityCreator(sceneManager, tileManager) {
	m_world = &connector->GetEngine().GetWorld();
}

EnemyEntityCreator::~EnemyEntityCreator() {

}

void EnemyEntityCreator::CreateSpawner(int id, glm::vec3 position, glm::vec3 rotation, int enemySpawnType, float enemySpawnDelay) {
	Frac::EntityHandle towerEntity = m_world->makeEntity("Spawners" + std::to_string(createdSpawners));
	//Frac::EntityHandle towerEntity = m_sceneManager->GetModel("Models/Models/Turrets/10.gltf", Frac::EModelFileType::GLTF);
	float entitySize = m_tileManager->m_tileMap->m_size - 0.25f;
	glm::vec2 enemySpawnerPos = { position.x, position.z };
	glm::vec3 WorldPos = m_tileManager->m_tileMap->GridPosToWorldPos(enemySpawnerPos);
	m_world->set<TOR::Transform>(towerEntity, { {WorldPos }, {glm::vec3(1,1,1)},{},{},{},{true},{true} });
	m_world->set<BoxComponent>(towerEntity, { {glm::vec4(0.79f, 0.93f, 0.26f, 1.f)}, true });
	m_world->set<GridPositionComponent>(towerEntity, { enemySpawnerPos });
	m_world->set<SpawnerComponent>(towerEntity, { GetEnemySpawnType(enemySpawnType) });
	m_tileManager->SetTileOccupancy(enemySpawnerPos, true, towerEntity);
	createdSpawners++;
	switch (id) {
	case 0: {
		
	}break;
	case 1: {

	}break;
	case 2: {

	}break;
	case 3: {

	}break;
	case 4: {

	}break;
	case 5: {

	}break;
	default:
		break;
	}
}

EEnemyType EnemyEntityCreator::GetEnemySpawnType(int type) {
	switch (type) {
	case 0: {
		return EEnemyType::Red;
	}
	break;
	case 1: {
		return EEnemyType::Blue;
	}break;
	case 2: {
		return EEnemyType::Green;
	}break;
	default:
		LOGINFO("invalid, returning standard spawning type");
		return EEnemyType::Red;
;		break;
	}
}
