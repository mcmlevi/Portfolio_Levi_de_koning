#include "Thpch.h"
#include <Events/EventMessenger.h>

Th::EventMessenger& Th::EventMessenger::GetInstance()
{
	static Th::EventMessenger messenger{};
	return messenger;
}

void Th::EventMessenger::RemoveMessenger(const std::string& key)
{
		m_eventDispatchers.erase(key);
}

bool Th::EventMessenger::CheckIfExists(const std::string& key)
{
	return (m_eventDispatchers.find(key) != m_eventDispatchers.end()) || (m_queuedEventDispatcher.find(key) != m_queuedEventDispatcher.end());
}
