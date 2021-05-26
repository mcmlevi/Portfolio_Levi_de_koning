#include "pch.h"
#include "LevelEntityCreator.h"


LevelEntityCreator::LevelEntityCreator(Frac::SceneManager* sceneManager, TileManager* tileManager, Frac::EngineConnector* engineConnector) : IEntityCreator(sceneManager, tileManager) {
		m_world = &engineConnector->GetEngine().GetWorld();
}

LevelEntityCreator::~LevelEntityCreator() {

}

void LevelEntityCreator::CreateStaticLevelModel(int level) {
	if (m_ActiveLevel != 0) {
		//Deloading level first
		//m_world->remove(m_ActiveLevel);
		m_ActiveLevel = 0;

	}
	switch (level)
	{
	case 0: {
		LOGINFO("Loading level 1 main asset : TODO ADD MODEL SUPPORT");
		m_ActiveLevel = m_sceneManager->GetModel("Models/Houdini_Prototypes/Level_Geo_Binary/Level1.glb", Frac::EModelFileType::GLB);
		m_world->set<TOR::Transform>(m_ActiveLevel, { {glm::vec3(0,0,0)}, {glm::vec3(1,1,1)},{},{},{},{true},{true} });
	}break;
	case 1: {
		LOGINFO("Loading level 2 main asset : TODO ADD MODEL SUPPORT");
		m_ActiveLevel = m_sceneManager->GetModel("Models/Houdini_Prototypes/Level_Geo_Binary/Level2.glb", Frac::EModelFileType::GLB);
		m_world->set<TOR::Transform>(m_ActiveLevel, { {glm::vec3(0,0,0)}, {glm::vec3(1,1,1)},{},{},{},{true},{true} });
	}break;
	case 2: {
		LOGINFO("Loading level 3 main asset : TODO ADD MODEL SUPPORT");
		m_ActiveLevel = m_sceneManager->GetModel("Models/Houdini_Prototypes/Level_Geo_Binary/Level3.glb", Frac::EModelFileType::GLB);
		m_world->set<TOR::Transform>(m_ActiveLevel, { {glm::vec3(0,0,0)}, {glm::vec3(1,1,1)},{},{},{},{true},{true} });
	}break;
	default:
		break;
	}
}

void LevelEntityCreator::SetLights(int level) {

}


