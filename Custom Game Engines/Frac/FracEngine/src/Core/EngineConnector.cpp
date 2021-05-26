#include "pch.h"
#include "Core\EngineConnector.h"
#include "Core\EngineCore.h"

using namespace Frac;

EngineCore& EngineConnector::GetEngine()
{
    // TODO: insert return statement here
    return EngineCore::getInstance();
}
