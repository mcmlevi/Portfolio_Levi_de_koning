#pragma once
#include "InputSystem/InputSystem.h"
#include "GLFW/glfw3.h"

#include <unordered_map>

namespace Frac
{

	class GLFWInput : public InputSystem
	{
	public:
		GLFWInput(GLFWwindow* );
		virtual ~GLFWInput();
		void Update(float dt) override;	
		bool GetKeyOnce(int key) override;
		bool GetKey(int key) override;
		
	protected:
		static void JoystickCallback(int joy, int event);
		static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
		
		void AddJoystick(int joy) override;
		
		std::unordered_map<int, float> m_joystickVibrationTimer{};

		GLFWwindow* m_windowPtr;

	private:
		static InputSystem* m_inputInstance;
	};

}
