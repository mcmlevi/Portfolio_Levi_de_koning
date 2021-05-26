#include "pch.h"
#include "PlayerManager.h"
#include "Core/EngineCore.h"

PlayerManager::PlayerManager(Frac::EngineConnector* connector, TileManager* tileManager, PlayerEntityCreator* playerEntityCreator) : m_connector(connector), m_tileManager(tileManager)
{
	m_playerEntityCreator = playerEntityCreator;
	m_resources = 100;
	m_turretCost = 1;
	m_selectedTile = { 0,0 };
	m_inputSystem = &m_connector->GetEngine().GetInputSystem();
	LOGINFO("Player Manager Initialized");
}

PlayerManager::~PlayerManager()
{
	m_connector = nullptr;
	m_tileManager = nullptr;
	m_inputSystem = nullptr;
}

void PlayerManager::SetBuildCosts(uint turretCost)
{
	m_turretCost = turretCost;
}

void PlayerManager::Update(glm::vec2 selectedTile)
{
	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
	m_selectedTile = selectedTile;
	
	//Actions
	if (m_tileManager->IsInGrid(m_selectedTile)) //Check if selected Tile is in the Grid.
	{
	
		//Check Input
		if ((m_inputSystem->GetKeyOnce(Frac::KEY_E) || m_inputSystem->GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_A)))
		{
			//BuildAction();
		}
		if ((m_inputSystem->GetKeyOnce(Frac::KEY_Q) || m_inputSystem->GetAnyJoystickButtonOnce(Frac::JOYSTICK_BUTTON_B)))
		{
			//SellAction();
		}
		
		
	}

}

void PlayerManager::BuildAction()
{
	//Build Turret using the TurretCreator
	//So, this is temporary:

	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
	float entitySize = m_tileManager->m_tileMap->m_size - 0.25;

	size_t currentTileID = m_tileManager->GetTileID(m_selectedTile);
	TileComponent* curTile = world.get<TileComponent>(m_tileManager->m_tileMap->m_tileEntities->at(currentTileID));

	if ((curTile->m_states.m_isBuildable) && (m_resources >= m_turretCost) && (!curTile->m_states.m_isWalkable)) //Check if Tile is Buildable
	{	

		m_playerEntityCreator->CreateTower(0, glm::vec3(m_selectedTile.x, 0, m_selectedTile.y) , glm::vec3(0, 0, 0), 100, 100); //TODO implement the height from the tile

		//Set Tile States
		curTile->m_states.m_isBuildable = false;

		//Set Resources
		RemoveResources(m_turretCost);

		LOGINFO("Player Build Turret at: %f , %f", m_selectedTile.x, m_selectedTile.y);
		
	}
}

void PlayerManager::SellAction()
{
	Frac::ECSWrapper& world = m_connector->GetEngine().GetWorld();
	EntityHandle curEntity = m_tileManager->m_tileMap->GetTileHandle(m_selectedTile);
	TileComponent* curTile = world.get<TileComponent>(curEntity);
	
	if ((curTile->m_states.m_isActive) && (curTile->m_occupier != 0) && (!curTile->m_states.m_isBuildable)) //TODO: Find why this doesn't work: (!curTile->m_states.m_isBuildable)
	{
		if (world.has(curTile->m_occupier) && world.has<TurretComponent>(curTile->m_occupier))
		{
			world.remove(curTile->m_occupier);
			m_tileManager->SetTileOccupancy(m_selectedTile, false, curEntity);
			curTile->m_states.m_isBuildable = true;
			AddResources(m_turretCost);
			LOGINFO("Player Sold Turret at: %f , %f", m_selectedTile.x, m_selectedTile.y);
		}		
	}

}

uint PlayerManager::GetResources()
{
	return m_resources;
}

void PlayerManager::SetResources(uint toSet)
{
	if (toSet < 0)
	{
		m_resources = 0;
	}
	else
	{
		m_resources = toSet;
	}
	//LOGINFO("Resources Set to: %i ", m_resources);
}

void PlayerManager::RemoveResources(uint toRemove)
{
	uint result = m_resources - toRemove;

	if (result < 0) 
		m_resources = 0;
	else 
		m_resources = result;

	//LOGINFO("Resources Removed. Resources remaining: %i ", m_resources);
}

void PlayerManager::AddResources(uint toAdd)
{
	m_resources += toAdd;
	//LOGINFO("Resources Added. Resources remaining: %i ", m_resources);
}
