#pragma once

namespace Frac
{
	/**
	* The GUI-window interface which serves as a base class for implementing a ImGui-window, managed by the GraphicalUserInterface system
	*/
	class IGUIWindow
	{
	public:
		IGUIWindow() : m_windowState(false) {}
		virtual ~IGUIWindow() = default;

		/**
		* Update loop with Window-specific implementation using the immediate mode GUI
		*/
		virtual void Update() = 0;

		/**
		* Sets the active state of the window
		*/
		void SetWindowState(bool a_state) {
			m_windowState = a_state;
		}

	protected:
		bool m_windowState;
	};
} // namespace Frac