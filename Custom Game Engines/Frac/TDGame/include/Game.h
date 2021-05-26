#pragma once
#include "Core\FrameListener.h"
#include "Core/ECSWrapper.h"
#include "GameManager.h"

class Game : public Frac::FrameListener {
public:
	Game();

	~Game();

	void FrameStarted() override;

	void FrameEnded() override;

	Frac::ECSWrapper* world;

private:
	std::unique_ptr<GameManager> m_GameManager;
};