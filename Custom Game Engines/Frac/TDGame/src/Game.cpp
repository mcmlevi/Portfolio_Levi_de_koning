#include "pch.h"
#include "Game.h"
#include "Core/EngineConnector.h"
#include "Core/EngineCore.h"
#include "Glm/gtx/transform.hpp"

#include "CoreRenderAPI/Components/Transform.h"
#include "Graphics/Components/TextComponent.h"

#include "Core/Query.h"

Game::Game() : Frac::FrameListener()
{
	GetEngineConnector().get()->GetEngine().Initialize();
	GetEngineConnector().get()->GetEngine().GetFrameListenerManager().AddFrameListener(this);
	world = &GetEngineConnector().get()->GetEngine().GetWorld();
	m_GameManager = std::make_unique <GameManager>(GetEngineConnector().get(), world);
	GetEngineConnector().get()->GetEngine().Run();	
	LOGINFO("Game Initialized");
}

Game::~Game()
{
	
}

void Game::FrameStarted()
{
	const glm::mat4 mainCamVP = m_GameManager->m_cameraManager->GetMainCameraEntityVP();
	GetEngineConnector()->GetEngine().GetTextRenderSystem().SetViewProjection(mainCamVP);
#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	m_GameManager->m_debugRenderer->Reset();
#endif

	m_GameManager->UpdateGame();	
}

void Game::FrameEnded()
{
	const glm::mat4 mainCamVP = m_GameManager->m_cameraManager->GetMainCameraEntityVP();

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	m_GameManager->m_debugRenderer->Render(mainCamVP);
	m_GameManager->DrawCursor();
#endif

}






