#include "pch.h"
#include "InputSystem/InputSystem.h"

using namespace Frac;

bool InputSystem::GetJoystickButton(Joystick joystick, JoystickButtons button)
{
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		return	state.Buttons[button];
	}
	return false;
}

bool InputSystem::GetJoystickButtonOnce(Joystick joystick, JoystickButtons button)
{
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		return	state.Buttons[button] && !state.LastButtons[button];
	}
	return false;
}

bool InputSystem::GetJoystickComboOnce(Joystick joystick, JoystickButtons button0, JoystickButtons button1)
{
	return	(GetJoystickButton(joystick, button0) && GetJoystickButtonOnce(joystick, button1)) ||
		(GetJoystickButton(joystick, button1) && GetJoystickButtonOnce(joystick, button0));
}

float InputSystem::GetJoystickAxis(Joystick joystick, JoystickAxes axis)
{
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		return state.Axes[axis];
	}
	return 0.0f;
}

std::vector<Joystick> InputSystem::GetActiveJoysticks() const
{
	std::vector<Joystick> active;
	for (auto j : m_joyState)
		active.push_back(j.first);

	return active;
}

Joystick InputSystem::GetDefaultJoystick() const
{
	if (m_joyState.empty())
		return Joystick();
	return m_joyState.begin()->first;
}

void InputSystem::RemoveJoystick(int joy)
{
	auto itr = m_joyState.find(joy);
	if (itr != m_joyState.end())
	{
		JoystickState& state = itr->second;
		state.Connected = false;
	}
}

bool InputSystem::GetLeft(Joystick joystick)
{
	const JoystickAxes axis = JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL;
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		if (state.Axes[axis] < -0.5f && state.LastAxes[axis] >= -0.5f)
			return true;
		if (GetJoystickButtonOnce(joystick, JOYSTICK_BUTTON_DPAD_LEFT))
			return true;
	}
	return false;
}

bool InputSystem::GetRight(Joystick joystick)
{
	const JoystickAxes axis = JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL;
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		if (state.Axes[axis] > 0.5f && state.LastAxes[axis] <= 0.5f)
			return true;
		if (GetJoystickButtonOnce(joystick, JOYSTICK_BUTTON_DPAD_RIGHT))
			return true;
	}
	return false;
}

bool InputSystem::GetUp(Joystick joystick)
{
	const JoystickAxes axis = JOYSTICK_AXIS_LEFT_THUMB_VERTICAL;
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		if (state.Axes[axis] < -0.5f && state.LastAxes[axis] >= -0.5f)
			return true;
		if (GetJoystickButtonOnce(joystick, JOYSTICK_BUTTON_DPAD_UP))
			return true;
	}
	return false;
}

bool InputSystem::GetDown(Joystick joystick)
{
	const JoystickAxes axis = JOYSTICK_AXIS_LEFT_THUMB_VERTICAL;
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		if (state.Axes[axis] > 0.5f && state.LastAxes[axis] <= 0.5f)
			return true;
		if (GetJoystickButtonOnce(joystick, JOYSTICK_BUTTON_DPAD_DOWN))
			return true;
	}
	return false;
}

bool InputSystem::GetAnyLeft()
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetLeft(joyState.first))
			{
				return true;
			}
		}
	}
	return false;
}

bool InputSystem::GetAnyRight()
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetRight(joyState.first))
			{
				return true;
			}
		}
	}
	return false;
}

bool InputSystem::GetAnyUp()
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetUp(joyState.first))
			{
				return true;
			}
		}
	}
	return false;
}

bool InputSystem::GetAnyDown()
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetDown(joyState.first))
			{
				return true;
			}
		}
	}
	return false;
}

bool InputSystem::GetAnyJoystickButton(JoystickButtons button)
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetJoystickButton(joyState.first, button))
			{
				return true;
			}
		}
	}
	return false;
}

bool InputSystem::GetAnyJoystickButtonOnce(JoystickButtons button)
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			if (GetJoystickButtonOnce(joyState.first, button))
			{
				return true;
			}
		}
	}
	return false;
}

float Frac::InputSystem::GetAnyJoystickAxis(JoystickAxes axis)
{
	if (!m_joyState.empty())
	{
		for(const std::map<int, JoystickState>::value_type joyState : m_joyState)
		{
			const float val = GetJoystickAxis(joyState.first, axis);
			if (val < -0.1f || val > 0.1f)
			{
				return val;
			}
		}
	}
	return false;
}

std::pair<Joystick, JoystickState&> InputSystem::AddVirtualJoystick()
{
	int joy = ++m_nextVirtual;

	JoystickState state;
	state.Name = "Virtual Joystick " + std::to_string(joy);
	state.Type = Frac::JoystickType::JOYSTICK_TYPE_VIRTUAL;
	m_joyState[joy] = state;
	
	return {joy, m_joyState[joy]};
}

JoystickType InputSystem::GetType(Joystick joystick) const
{
	auto itr = m_joyState.find(joystick.GetJoystickID());
	if (itr != m_joyState.end())
	{
		auto& state = itr->second;
		return state.Type;
	}
	return JoystickType::JOYSTICK_TYPE_INVALID;
}

