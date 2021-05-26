#include "Thpch.h"
#include <Input/IController.h>

const Th::ControllerState& IController::GetState()
{
	return m_controllerState;
}
