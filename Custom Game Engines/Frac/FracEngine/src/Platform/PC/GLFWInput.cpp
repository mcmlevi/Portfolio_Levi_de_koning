#include "pch.h"
#include <Platform/PC/GLFWInput.h>
#include <Platform/PC/PCDevice.h>

#include "Logger.h"
#include "Core\EngineCore.h"

using namespace Frac;

InputSystem* Frac::GLFWInput::m_inputInstance = nullptr;
static glm::vec2 scrollOffset = glm::vec2(0.f); // find another way????

std::unordered_map<JoystickButtons, int> virtualMapping =
{
	{ JOYSTICK_BUTTON_A,			GLFW_KEY_SPACE },
	{ JOYSTICK_BUTTON_B,			GLFW_KEY_ESCAPE },
	{ JOYSTICK_BUTTON_X,			GLFW_KEY_LEFT_CONTROL },
	{ JOYSTICK_BUTTON_Y,			GLFW_KEY_LEFT_SHIFT },
	{ JOYSTICK_BUTTON_LB,			GLFW_KEY_0 },
	{ JOYSTICK_BUTTON_RB,			GLFW_KEY_0 },
	{ JOYSTICK_BUTTON_BACK,			GLFW_KEY_ESCAPE },
	{ JOYSTICK_BUTTON_START,		GLFW_KEY_ENTER },
	{ JOYSTICK_BUTTON_DUMMY,		GLFW_KEY_X },
	{ JOYSTICK_BUTTON_L3,			GLFW_KEY_0 },
	{ JOYSTICK_BUTTON_R3,			GLFW_KEY_0 },
	{ JOYSTICK_BUTTON_DPAD_UP,		GLFW_KEY_UP },
	{ JOYSTICK_BUTTON_DPAD_RIGHT,	GLFW_KEY_RIGHT },
	{ JOYSTICK_BUTTON_DPAD_DOWN,	GLFW_KEY_DOWN },
	{ JOYSTICK_BUTTON_DPAD_LEFT,	GLFW_KEY_LEFT },
};

void GLFWInput::JoystickCallback(int joy, int event)
{
	if (event == GLFW_CONNECTED && glfwJoystickIsGamepad(joy))
	{
		dynamic_cast<GLFWInput&>(*m_inputInstance).AddJoystick(joy);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		dynamic_cast<GLFWInput&>(*m_inputInstance).RemoveJoystick(joy);
	}
}

void GLFWInput::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollOffset = glm::vec2(xoffset, yoffset);
}

GLFWInput::GLFWInput(GLFWwindow* windowPtr) : m_windowPtr(windowPtr), InputSystem()
{
	m_inputInstance = this;
	
	glfwSetJoystickCallback(JoystickCallback);
	glfwSetScrollCallback(windowPtr, ScrollCallback);
	
	for (int i = GLFW_JOYSTICK_1; i < GLFW_JOYSTICK_LAST; i++)
	{
		if (glfwJoystickPresent(i) && glfwJoystickIsGamepad(i))
			GLFWInput::AddJoystick(i);
	}
}

GLFWInput::~GLFWInput() = default;

void GLFWInput::Update(float dt)
{	
	if (m_joyState.empty())
		return;

	for (auto i = m_joyState.begin(); i != m_joyState.end();)
	{
		if (!i->second.Connected)
			i = m_joyState.erase(i);
		else
			++i;
	}

	for (auto& i : m_joyState)
	{
		int joy = i.first;
		JoystickState& state = i.second;

		// Virtual
		if (joy >= 256)
		{
			const float f = 0.1f;

			for (int j = 0; j < JOYSTICK_AXIS_COUNT; j++)
				state.LastAxes[j] = state.Axes[j];

			state.Axes[JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL]		=  static_cast<float>(GetKey(GLFW_KEY_D) - GetKey(GLFW_KEY_A));
			state.Axes[JOYSTICK_AXIS_LEFT_THUMB_VERTICAL]		= -static_cast<float>(GetKey(GLFW_KEY_W) - GetKey(GLFW_KEY_S));
			state.Axes[JOYSTICK_AXIS_RIGHT_THUMB_HORIZONTAL]	=  static_cast<float>(GetKey(GLFW_KEY_RIGHT) - GetKey(GLFW_KEY_LEFT));
			state.Axes[JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL]		= -static_cast<float>(GetKey(GLFW_KEY_UP) - GetKey(GLFW_KEY_DOWN));
			
			state.Axes[JOYSTICK_AXIS_RIGHT_TRIGGER] = static_cast<float>(GetKey(GLFW_KEY_UP));

			for (int j = 0; j < JOYSTICK_BUTTON_COUNT; j++)
			{
				state.LastButtons[j] = state.Buttons[j];
				state.Buttons[j] = GetKey(virtualMapping[static_cast<JoystickButtons>(j)]);
			}
		}
		else
		{
			GLFWgamepadstate gamepadState;
			if (glfwGetGamepadState(joy, &gamepadState))
			{
				state.LastAxes.assign(state.Axes.begin(), state.Axes.end());
				state.Axes.assign(gamepadState.axes, gamepadState.axes + JOYSTICK_AXIS_COUNT);
				state.LastButtons.assign(state.Buttons.begin(), state.Buttons.end());
				state.Buttons.assign(gamepadState.buttons, gamepadState.buttons + JOYSTICK_BUTTON_COUNT);
				// Fix GLFW's format
				state.Axes[JOYSTICK_AXIS_LEFT_TRIGGER] = (state.Axes[JOYSTICK_AXIS_LEFT_TRIGGER] + 1.0f) * 0.5f;
				state.Axes[JOYSTICK_AXIS_RIGHT_TRIGGER] = (state.Axes[JOYSTICK_AXIS_RIGHT_TRIGGER] + 1.0f) * 0.5f;
				state.LastAxes[JOYSTICK_AXIS_LEFT_TRIGGER] = (state.LastAxes[JOYSTICK_AXIS_LEFT_TRIGGER] + 1.0f) * 0.5f;
				state.LastAxes[JOYSTICK_AXIS_RIGHT_TRIGGER] = (state.LastAxes[JOYSTICK_AXIS_RIGHT_TRIGGER] + 1.0f) * 0.5f;
			}
			else
			{
				state.Connected = false;
			}
		}
	}

	double mx, my;
	auto* const window = m_windowPtr;
	glfwGetCursorPos(window, &mx, &my);
	m_mouse.m_lastPosition.x = m_mouse.m_position.x;
	m_mouse.m_lastPosition.y = m_mouse.m_position.y;
	m_mouse.m_position.x = static_cast<float>(mx);
	m_mouse.m_position.y = static_cast<float>(my);
	m_mouse.m_down[0] = glfwGetMouseButton(window, 0) == GLFW_PRESS;
	m_mouse.m_down[1] = glfwGetMouseButton(window, 1) == GLFW_PRESS;
	m_mouse.m_down[2] = glfwGetMouseButton(window, 2) == GLFW_PRESS;
	m_mouse.m_wheel = scrollOffset.y;
	scrollOffset = { 0.f,0.f }; // Reset the scroll ofset so we dont infinitly zoom
}

void GLFWInput::AddJoystick(int joy)
{
	std::string name(glfwGetGamepadName(joy));
	JoystickState state;
	state.Name = name + " " + std::to_string(joy);
	LOGINFO("Joystick {%s} connected.", state.Name);
	
	//! NOTE: I do not know all the exact names for the controllers since I do not own all of them, please confirm these names
	//! please leave a //[X] behind confirmed names.
	if (name == "PS4 Controller")				state.Type = JoystickType::JOYSTICK_TYPE_PLAYSTATION;	//[X]
	else if (name == "Steam Controller")		state.Type = JoystickType::JOYSTICK_TYPE_STEAM;
	else if (name == "XInput Gamepad (GLFW)")	state.Type = JoystickType::JOYSTICK_TYPE_XBOX_360;		//[X]
	else if (name == "Xbox One Controller")		state.Type = JoystickType::JOYSTICK_TYPE_XBOX;
	else if (name == "unknown")					state.Type = JoystickType::JOYSTICK_TYPE_INVALID;
	m_joyState[joy] = state;
}

bool GLFWInput::GetKeyOnce(int key)
{
	return (glfwGetKey(m_windowPtr, key) ?
		(m_keyOnce[key] ? false : (m_keyOnce[key] = true)) : \
		(m_keyOnce[key] = false));
}

bool GLFWInput::GetKey(int key)
{
	return glfwGetKey(m_windowPtr, key) == GLFW_PRESS;
}
