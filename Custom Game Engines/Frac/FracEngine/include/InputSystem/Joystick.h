#pragma once

namespace Frac
{
	/*!
	 * @brief Holds all possible buttons a joystick has.
	 * @note This is the default order matching a Xbox controller
	 */
	enum JoystickButtons
	{
		
		JOYSTICK_BUTTON_A = 0,
		JOYSTICK_BUTTON_B = 1,
		JOYSTICK_BUTTON_X = 2,
		JOYSTICK_BUTTON_Y = 3,
		JOYSTICK_BUTTON_LB = 4,
		JOYSTICK_BUTTON_RB = 5,
		JOYSTICK_BUTTON_BACK = 6,
		JOYSTICK_BUTTON_START = 7,
		JOYSTICK_BUTTON_DUMMY = 8,
		JOYSTICK_BUTTON_L3 = 9,
		JOYSTICK_BUTTON_R3 = 10,
		JOYSTICK_BUTTON_DPAD_UP = 11,
		JOYSTICK_BUTTON_DPAD_RIGHT = 12,
		JOYSTICK_BUTTON_DPAD_DOWN = 13,
		JOYSTICK_BUTTON_DPAD_LEFT = 14,
		JOYSTICK_BUTTON_COUNT
	};

	/*!
	 * @brief Holds the type of axes a controller might have.
	 * @note Contains both joystick axes and triggers
	 */
	enum JoystickAxes
	{
		// Default order matching Xbox controller
		JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL = 0,
		JOYSTICK_AXIS_LEFT_THUMB_VERTICAL,			// 1
		JOYSTICK_AXIS_RIGHT_THUMB_HORIZONTAL,		// 2
		JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL,			// 3
		JOYSTICK_AXIS_LEFT_TRIGGER,					// 4
		JOYSTICK_AXIS_RIGHT_TRIGGER,				// 5	
		JOYSTICK_AXIS_COUNT							// 6
	};
	
	/*!
	* @brief Holds the type of joystick a joystick might be.
	* @note Can be further expanded based on required joystick type
	*/
	enum class JoystickType
	{
		JOYSTICK_TYPE_XBOX,
		JOYSTICK_TYPE_XBOX_360,
		JOYSTICK_TYPE_PLAYSTATION,
		JOYSTICK_TYPE_STEAM,	
		JOYSTICK_TYPE_NINTENDO_LEFT_JOYCON,
		JOYSTICK_TYPE_NINTENDO_RIGHT_JOYCON,
		JOYSTICK_TYPE_NINTENDO_DUAL_JOYCONS,
		JOYSTICK_TYPE_NINTENDO_FULLKEY,
		JOYSTICK_TYPE_NINTENDO_HANDHELD,
		JOYSTICK_TYPE_VIRTUAL,
		JOYSTICK_TYPE_INVALID
	};

	/*!
	 * @brief The JoystickState holds everything related to joysticks, this structure is stored and updated in the InputSystem
	 */
	struct JoystickState
	{
		std::string			Name;
		std::vector<float>	Axes;
		std::vector<int>	Buttons;
		std::vector<float>	LastAxes;
		std::vector<int>	LastButtons;
		bool				Connected;
		bool				Rumble;
		JoystickType		Type;
		JoystickState()
			:	Axes(8),
			Buttons(24),
			LastAxes(8),
			LastButtons(24),
			Connected(true),
			Rumble(false),
			Type(JoystickType::JOYSTICK_TYPE_INVALID)
		{}
	};
	
	/*!
	* @brief The Joystick class. This class only holds an identifier used by the InputSystem for query actions.
	*/
	class Joystick
	{
	public:
		Joystick() { m_joystick = -1; }
		Joystick(const Joystick& j) { m_joystick = j.m_joystick; }	
		Joystick(const int j) { m_joystick = j; }

		/*!
		 * @brief Checks if this joystick is valid
		 * @return Valid state
		 */
		bool IsValid() const { return m_joystick != -1; }
		/*!
		* @brief Checks if this joystick is virtual. Virtual joysticks can be used for debugging purposes
		* @return Virtual state
		*/
		bool IsVirtual() const { return m_joystick >= 256; }
		/*!
		 * @brief Gets the Joystick identifier, this identifier can be used
		 * @return Returns the joystick identifier
		 */
		int GetJoystickID() const { return m_joystick; }
		
	protected:
		int m_joystick;
	};
}
