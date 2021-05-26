#pragma once
#include <Input/ControllerState.h>

class IController
{
public:
	IController() = default;
	virtual ~IController() = default;
	virtual void InitializeController() = 0;
	virtual void UpdateStatus() = 0;

	const Th::ControllerState& GetState();
	
protected:
	Th::ControllerState m_controllerState{};
};

