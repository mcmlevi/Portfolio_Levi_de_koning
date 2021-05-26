#include "Thpch.h"
#include <Events/InputHandler.h>
#ifdef PLATFORM_WIN64
#include <Input/WinController.h>
typedef  Th::WinController Controller;
#elif PLATFORM_PS4
#include <Input/PS4Controller.h>
typedef  Th::PS4Controller Controller;
#endif


namespace Th
{
	InputHandler& InputHandler::getInstance()
	{
		static InputHandler instance;
		return instance;
	}

	void InputHandler::ShutDown()
	{
		for (int i = 0; i < m_controllers.size(); ++i)
		{
			delete m_controllers[i].second;
			m_controllers[i].second = nullptr;
		}
	}

	void InputHandler::UpdateInput()
	{
		
		std::vector<const Th::ControllerState*> controllerStates;
		controllerStates.resize(m_controllers.size());
		// Poll all the controllers and get their state
		for (int i = 0; i < m_controllers.size(); ++i)
		{
			m_controllers[i].second->UpdateStatus();
			controllerStates[i] = &m_controllers[i].second->GetState();
		}
		// Evoke the controller event
		m_ControllerStateEvent(controllerStates);
	}

	void InputHandler::InvokeKeyPressedEvent(Th::KeyEventArgs& args)
	{
		m_keyPressedEvent(args);
	}

	void InputHandler::InvokeKeyReleasedEvent(Th::KeyEventArgs& args)
	{
		m_keyReleasedEvent(args);
	}

	void InputHandler::InvokeMouseMotionEvent(Th::MouseMotionEventArgs& args)
	{
		m_mouseMotionEvent(args);
	}

	void InputHandler::InvokeMouseButtonPressedEvent(Th::MouseButtonEventArgs& args)
	{
		m_mouseButtonPressedEvent(args);
	}

	void InputHandler::InvokeMouseButtonReleasedEvent(Th::MouseButtonEventArgs& args)
	{
		m_mouseButtonReleasedEvent(args);
	}

	void InputHandler::InvokeMouseWheelEvent(Th::MouseWheelEventArgs& args)
	{
		m_mouseWheelEvent(args);
	}

	KeyEvent& Th::InputHandler::GetKeyPresedEventDelegate()
	{
		return m_keyPressedEvent;
	}

	KeyEvent& InputHandler::GetKeyReleasedEventDelegate()
	{
		return m_keyReleasedEvent;
	}

	MouseMotionEvent& InputHandler::GetMouseMotionDelegate()
	{
		return m_mouseMotionEvent;
	}

	MouseButtonEvent& Th::InputHandler::GetMouseButtonPressedDelegate()
	{
		return m_mouseButtonPressedEvent;
	}

	MouseButtonEvent& InputHandler::GetMouseButtonReleasedDelegate()
	{
		return m_mouseButtonReleasedEvent;
	}

	MouseWheelEvent& Th::InputHandler::GetMouseWheelDelegate()
	{
		return m_mouseWheelEvent;
	}

	ControllerStateEvent& InputHandler::GetControllerStateDelegate()
	{
		return 	m_ControllerStateEvent;
	}

	int InputHandler::AddController()
	{
		IController* ptr = new Controller{};
		ptr->InitializeController();
		m_controllers.push_back(std::make_pair(m_controllerID++,ptr));
		return m_controllerID - 1;
	}

	void InputHandler::RemoveController(int id)
	{
		bool removedElement{ false };
		for (int i = 0; i < m_controllers.size(); ++i)
		{
			if(m_controllers[i].first == id)
			{
				removedElement = true;
				m_controllers.erase(m_controllers.begin() + i);
				break;
			}
		}
		assert(removedElement);
	}
}
