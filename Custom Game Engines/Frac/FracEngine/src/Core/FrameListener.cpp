#include "pch.h"
#include "Core\FrameListener.h"

using namespace Frac;

std::unique_ptr<EngineConnector>& FrameListener::GetEngineConnector()
{
    return m_engineConnector;
}
