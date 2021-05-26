#pragma once
#include <unordered_map>
#include <string>
#include <any>
#include <queue>

#include <Events/Delegate.h>

namespace Th
{
	class EventMessenger
	{
	public:

		/**
		 * \brief Get the event messenger instance
		 * \return reference to the event messenger instance
		 */
		static  EventMessenger& GetInstance();
		
		/**
		 * \brief Create a new event messenger
		 * \tparam Args the arguments the event messenger will take
		 * \param eventName the name of the event 
		 */
		template<typename... Args>
		void AddMessenger(const std::string& eventName);
		template<typename Arg>
		void AddQueuedMessenger(const std::string& eventName);
		/**
		 * \brief Remove a messenger from the list
		 * \param key to the messenger to remove
		 */
		void RemoveMessenger(const std::string& key);

		/**
		 * \brief Evoke the messenger and call all subscribed functions, if the arguments and messenger arguments are not matching you will have een bad time.... as it uses the arguments to cast it back
		 * \tparam Args arguments to pass in the messengers also used to cast can be automatically deduced
		 * \param key the handle to the event 
		 * \param arguments arguments to pass in the messenger
		 */
		template<typename... Args>
		void EvokeMessenger(const std::string& key, Args&&... arguments);
		/**
		 * \brief Add an message to the queue.
		 * \tparam Arg the argument type to store in the queue
		 * \param key the key to the queued event.
		 * \param argument the value to store in the queue
		 */
		template <typename Arg>
		void AddQueuedMessage(const std::string& key, Arg& argument);
		/**
		 * \brief Flush all queued messages from an event and evoke all connected callbacks
		 * \tparam Arg 
		 * \param key 
		 */
		template <typename Arg>
		void Flush(const std::string& key);
		/**
		 * \brief connect a listener to the messenger
		 * \tparam Args used to cast the messenger to the correct type.
		 * \tparam Func the function to connect can automatically deduced
		 * \param key the key to the messenger
		 * \param f the function to connect
		 */
		template<typename... Args, typename Func>
		void ConnectListener(const std::string& key, Func&& f);
		/**
		 * \brief connect a listener to the messenger
		 * \tparam Args used to cast the messenger to the correct type
		 * \tparam Func the function to connect can be automatically deduced
		 * \tparam Instance the class instance to connect can be automatically deduced
		 * \param key the key to the messenger
		 * \param f the function to connect
		 * \param instance the class instance to connect
		 */
		template<typename... Args, typename Func, typename Instance>
		void ConnectListener(const std::string& key, Func&& f, Instance&& instance);
		/**
		 * \brief Disconnect a listener from the messenger
		 * \tparam Args used to cast the messenger to the correct type
		 * \tparam Func the function to disconnect can be automatically deduced 
		 * \param key the key to the messenger
		 * \param f the function to disconnect
		 */
		template<typename... Args, typename Func>
		void DisconnectListener(const std::string& key, Func&& f);
		/**
		 * \brief Disconnect a listener from the messenger
		 * \tparam Args used to cast the messenger to the correct type
		 * \tparam Func the function to disconnect can be automatically deduced
		 * \tparam Instance the instance of the class to disconnect can be automatically deduced
		 * \param key the key to the messenger
		 * \param f the function to disconnect
		 * \param instance the class instance
		 */
		template<typename... Args, typename Func, typename Instance>
		void DisconnectListener(const std::string& key, Func&& f, Instance&& instance);
		/**
		* \brief Evoke the messenger and call all subscribed functions, if the arguments and messenger arguments are not matching you will have een bad time.... as it uses the arguments to cast it back
		* \tparam Args arguments to pass in the messengers also used to cast can be automatically deduced
		* \param key the handle to the event
		* \param arguments arguments to pass in the messenger
		*/
		template<typename... Args>
		void operator()(const std::string& key, Args&&... arguments);
	private:
		bool CheckIfExists(const std::string& key);
		typedef std::pair<std::queue<std::any>, std::any> queuedEvent;
		std::unordered_map<std::string, std::any> m_eventDispatchers;
		std::unordered_map<std::string, queuedEvent> m_queuedEventDispatcher;
	};

	template <typename... Args>
	void EventMessenger::AddMessenger(const std::string& eventName)
	{
		m_eventDispatchers.insert(std::make_pair( eventName, new Delegate<Args&&...>{}));
	}

	template <typename Arg>
	void EventMessenger::AddQueuedMessenger(const std::string& eventName)
	{
		m_queuedEventDispatcher.insert(std::make_pair(eventName, queuedEvent{{},new Delegate<Arg>{}}));
	}


	template <typename... Args>
	void EventMessenger::EvokeMessenger(const std::string& key, Args&&... arguments)
	{
		assert(CheckIfExists(key));
		auto delegate = std::any_cast<Delegate<Args&&...>*>(m_eventDispatchers[key]);
		(*delegate)(std::forward<Args>(arguments)...);
	}

	template <typename Arg>
	void EventMessenger::AddQueuedMessage(const std::string& key, Arg& argument)
	{
		assert(CheckIfExists(key));
		m_queuedEventDispatcher[key].first.push(argument);
	}

	template <typename Arg>
	void EventMessenger::Flush(const std::string& key)
	{
		assert(CheckIfExists(key));
		auto delegate = std::any_cast<Delegate<Arg>*>(m_queuedEventDispatcher[key].second);
		auto queue = m_queuedEventDispatcher[key].first;
		while (!queue.empty())
		{
			auto& event = queue.front();
			(*delegate)(std::any_cast<Arg>(event));
			queue.pop();
		}
	}

	template <typename... Args, typename Func>
	void EventMessenger::ConnectListener(const std::string& key, Func&& f)
	{
		assert(CheckIfExists(key));
		if(m_eventDispatchers.find(key) != m_eventDispatchers.end())
		{
			auto delegate = std::any_cast<Delegate<Args&&...>*>(m_eventDispatchers[key]);
			delegate->Connect(f);
		}
		if(m_queuedEventDispatcher.find(key) != m_queuedEventDispatcher.end())
		{
			auto delegate = std::any_cast<Delegate<Args...>*>(m_queuedEventDispatcher[key].second);
			delegate->Connect(f);
		}
	}

	template <typename... Args, typename Func, typename Instance>
	void EventMessenger::ConnectListener(const std::string& key, Func&& f, Instance&& instance)
	{
		assert(CheckIfExists(key));
		if (m_eventDispatchers.find(key) != m_eventDispatchers.end())
		{
			auto delegate = std::any_cast<Delegate<Args&&...>*>(m_eventDispatchers[key]);
			delegate->Connect(f, instance);
		}
		else if (m_queuedEventDispatcher.find(key) != m_queuedEventDispatcher.end())
		{
			auto delegate = std::any_cast<Delegate<Args...>*>(m_queuedEventDispatcher[key].second);
			delegate->Connect(f, instance);
		}
	}

	template <typename... Args, typename Func>
	void EventMessenger::DisconnectListener(const std::string& key, Func&& f)
	{
		assert(CheckIfExists(key));
		if (m_eventDispatchers.find(key) != m_eventDispatchers.end())
		{
			auto delegate = std::any_cast<Delegate<Args&&...>*>(m_eventDispatchers[key]);
			delegate->Disconnect(f);
		}
		else if (m_queuedEventDispatcher.find(key) != m_queuedEventDispatcher.end())
		{
			auto delegate = std::any_cast<Delegate<Args...>*>(m_queuedEventDispatcher[key].second);
			delegate->Disconnect(f);
		}
	}

	template <typename... Args, typename Func, typename Instance>
	void EventMessenger::DisconnectListener(const std::string& key, Func&& f, Instance&& instance)
	{
		assert(CheckIfExists(key));
		if (m_eventDispatchers.find(key) != m_eventDispatchers.end())
		{
			auto delegate = std::any_cast<Delegate<Args&&...>*>(m_eventDispatchers[key]);
			delegate->Disconnect(f, instance);
		}
		else if (m_queuedEventDispatcher.find(key) != m_queuedEventDispatcher.end())
		{
			auto delegate = std::any_cast<Delegate<Args...>*>(m_queuedEventDispatcher[key].second);
			delegate->Disconnect(f, instance);
		}
	}

	template <typename ... Args>
	void EventMessenger::operator()(const std::string& key, Args&&... arguments)
	{
		EvokeMessenger(key,std::forward<Args>(arguments)...);
	}
}
