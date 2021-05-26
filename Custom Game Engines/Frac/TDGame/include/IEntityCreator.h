#pragma once
#include "TileManager.h"
#include <glm\vec2.hpp>
#include "SceneManagement/SceneManager.h"
#include "Glm/vec3.hpp"
#include "Core/EngineConnector.h"
#include "Core/EngineCore.h"
#include "TileManager.h"
class IEntityCreator
{

public:
IEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager);
~IEntityCreator();
bool CheckAvailability(glm::vec2 position);
Frac::SceneManager* m_sceneManager;
TileManager* m_tileManager;

private:
	
};
