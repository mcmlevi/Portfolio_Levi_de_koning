 /*
 * \class Player Manager
 *
 * \brief Responsible for handling Functions and holding Data that have to do with our User.
 *		  e.g, Build, Sell and more Input Related tasks.
 *
 * \author Antreas
 * \date April 2021
 */

#pragma once
#include "Components.h"
#include "Core/EngineConnector.h"
#include "InputSystem/InputSystem.h"
#include "TileManager.h"
#include "PlayerEntityCreator.h"

class PlayerManager
{
public:

	PlayerManager(Frac::EngineConnector* connector, TileManager* tileManager, PlayerEntityCreator* playerEntityCreator);

	~PlayerManager();

	void SetBuildCosts(uint turretCost);

	void Update(glm::vec2 selectedTile);
	void BuildAction();
	void SellAction();

	uint GetResources();
	void SetResources(uint toSet);
	void RemoveResources(uint toRemove);
	void AddResources(uint toAdd);

private:
	glm::vec2 m_selectedTile;
	uint m_resources;
	uint m_turretCost;
	Frac::EngineConnector* m_connector;
	Frac::InputSystem* m_inputSystem;
	TileManager* m_tileManager;
	PlayerEntityCreator* m_playerEntityCreator; 
};

