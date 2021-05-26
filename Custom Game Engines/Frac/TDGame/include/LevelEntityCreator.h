#pragma once
#include "IEntityCreator.h"
#include "Components.h"
class LevelEntityCreator : public IEntityCreator
{
public:
LevelEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* connector);
~LevelEntityCreator();
void CreateStaticLevelModel(int level);
void SetLights(int level);
private:
	EntityHandle m_ActiveLevel = 0;
	Frac::ECSWrapper* m_world;
};

