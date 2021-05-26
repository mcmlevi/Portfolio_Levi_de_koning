#pragma once
#include <map>
#include <vector>
#include <Glm/ext/vector_float2.hpp>

#include "Joystick.h"

namespace Frac
{
	/*!
	 * @brief Straight from GLFW
	 */
	enum KEYS
	{
		KEY_SPACE				= 32,
		KEY_ESCAPE				= 256,
		KEY_ENTER				= 257,
		KEY_RIGHT				= 262,
		KEY_LEFT				= 263,
		KEY_DOWN				= 264,
		KEY_UP					= 265,
		KEY_APOSTROPHE			= 39,  /* ' */
		KEY_COMMA				= 44,  /* , */
		KEY_MINUS				= 45,  /* - */
		KEY_PERIOD				= 46,  /* . */
		KEY_SLASH				= 47,  /* / */
		KEY_0					= 48,
		KEY_1					= 49,
		KEY_2					= 50,
		KEY_3					= 51,
		KEY_4					= 52,
		KEY_5					= 53,
		KEY_6					= 54,
		KEY_7					= 55,
		KEY_8					= 56,
		KEY_9					= 57,
		KEY_SEMICOLON			= 59,  /* ; */
		KEY_EQUAL				= 61,  /* = */
		KEY_A					= 65,
		KEY_B					= 66,
		KEY_C					= 67,
		KEY_D					= 68,
		KEY_E					= 69,
		KEY_F					= 70,
		KEY_G					= 71,
		KEY_H					= 72,
		KEY_I					= 73,
		KEY_J					= 74,
		KEY_K					= 75,
		KEY_L					= 76,
		KEY_M					= 77,
		KEY_N					= 78,
		KEY_O					= 79,
		KEY_P					= 80,
		KEY_Q					= 81,
		KEY_R					= 82,
		KEY_S					= 83,
		KEY_T					= 84,
		KEY_U					= 85,
		KEY_V					= 86,
		KEY_W					= 87,
		KEY_X					= 88,
		KEY_Y					= 89,
		KEY_Z					= 90,
		KEY_LEFT_BRACKET		= 91,  /* [ */
		KEY_BACKSLASH			= 92,  /* \ */
		KEY_RIGHT_BRACKET		= 93,  /* ] */
		KEY_F1					= 290,
		KEY_F2					= 291,
		KEY_F3					= 292,
		KEY_F4					= 293,
		KEY_F5					= 294,
		KEY_F6					= 295,
		KEY_F7					= 296,
		KEY_F8					= 297,
		KEY_F9					= 298,
		KEY_F10					= 299,
		KEY_F11					= 300,
		KEY_F12					= 301,
		KEY_LEFT_SHIFT			= 340,
		KEY_LEFT_CONTROL		= 341,
		KEY_LEFT_ALT			= 342,
		KEY_RIGHT_SHIFT			= 344,
		KEY_RIGHT_CONTROL		= 345,
		KEY_RIGHT_ALT			= 346,
		KEY_CAPS_LOCK			= 280,
		KEY_SCROLL_LOCK			= 281,
		KEY_NUM_LOCK			= 282,
		KEY_PRINT_SCREEN		= 283,
		KEY_PAUSE				= 284
	};

	/*!
	 * @brief MouseState specifies the state of a mouse.
	 * @see InputSystem.GetMouse()
	 */
	struct MouseState
	{
		glm::vec2 m_position; /*!< Holds screen position*/
		glm::vec2 m_lastPosition; /*!< Holds last known screen position*/
		float m_wheel = 0.0f; /*!< Mouse wheel offset */
		bool m_down[3] = { false, false, false }; /*!< LMB, RMB & MMB down states.*/
		bool m_isValid = true;
	};

	/*!
	 * @brief The InputSystem is responsible for the updating and handling of queries relating to keyboard, joystick and mouse states. Has virtual abstract methods, relies on cross platform implementations.
	 */
	class InputSystem
	{
	public:
		virtual ~InputSystem() = default;
		/*!
		 * @brief Updates Keyboard, Mouse, Joystick States
		 * @param dt takes in delta-time
		 */
		virtual void Update(float dt) = 0;
		/*!
		 * @brief Gets state of a keyboard key of first initial press
		 * @see JoystickButtons
		 * @param key Key to check for. See Frac::KEYS
		 * @return bool press state
		 */
		virtual bool GetKeyOnce(int key) = 0;
		/*!
		* @brief Gets state of a keyboard key
		* @param key Key to check for. See Frac::KEYS
		* @return bool press state
		* @note Only works when the keyOnce is set through the Update()
		*/
		virtual bool GetKey(int key) = 0;
		/*!
		* @brief Returns button state of a certain joystick
		* @param joystick The joystick to test
		* @param button The button to query
		* @return bool press state
		*/
		bool GetJoystickButton(Joystick joystick, JoystickButtons button);
		/*!
		* @brief Returns button state of a certain joystick on first initial press.
		* @param joystick The joystick to test
		* @param button The button to query
		* @note Only works when a lastButton is set through the Update()
		* @return bool press state
		*/
		bool GetJoystickButtonOnce(Joystick joystick, JoystickButtons button);
		/*!
		* @brief Checks state of two buttons and returns true on first initial press of a certain joystick. See JoystickButtons.
		* @param joystick The joystick to test
		* @param button0 First button to query
		* @param button1 Second button to query
		* @note Only works when a lastButton is set through the Update()
		* @return true if both button0 and button1 are pressed
		*/
		bool GetJoystickComboOnce(Joystick joystick, JoystickButtons button0, JoystickButtons button1);
		/*!
		* @brief Checks state of a joystick axis. see Frac::JoystickAxes
		* @param joystick The joystick to test
		* @param axis The axis to query. Can be Triggers
		* @return a floating point number ranging from -1.0 to 1.0
		*/
		float GetJoystickAxis(Joystick joystick, JoystickAxes axis);
		/*!
		* @brief Gets the joysticks that are currently registered and active.
		* @return A vector of joysticks
		*/
		std::vector<Joystick> GetActiveJoysticks() const;
		/*!
		* @brief Gets the first registered joystick.
		* @return The default joystick.
		*/
		Joystick GetDefaultJoystick() const;
		/*!
		 * @brief Adds a virtual joystick. This joystick can be used for debugging purposes. This is the only joystick variant that allows it's state to be manually edited.
		 * @return Returns a pair consisting of the joystick identifier and a reference to it's state.
		 * @note Although the state of this joystick can be manually edited, it is set by the Windows specific GLFWInput implementation to simulate a joystick with keyboard keys.
		 */
		std::pair<Joystick, JoystickState&> AddVirtualJoystick();
		/*!
		 * @brief Gets Left, be it from d-pad or joystick axes.
		 * @param joystick checks the left state based on passed Joystick
		 * @return returns the pressed state of button/axes
		 */
		bool GetLeft(Joystick joystick);
		/*!
		* @brief Gets Right, be it from d-pad or joystick axes.
		* @param joystick checks the right state based on passed Joystick
		* @return returns the pressed state of button/axes
		*/
		bool GetRight(Joystick joystick);
		/*!
		* @brief Gets Up, be it from d-pad or joystick axes.
		* @param joystick checks the up state based on passed Joystick
		* @return returns the pressed state of button/axes
		*/
		bool GetUp(Joystick joystick);
		/*!
		* @brief Gets Down, be it from d-pad or joystick axes.
		* @param joystick checks the down state based on passed Joystick
		* @return returns the pressed state of button/axes
		*/
		bool GetDown(Joystick joystick);
		/*!
		* @brief Gets Any Left, be it from d-pad or joystick axes.
		* @return returns the pressed state of button/axes
		*/
		bool GetAnyLeft();
		/*!
		* @brief Gets Any Right, be it from d-pad or joystick axes.
		* @return returns the pressed state of button/axes
		*/
		bool GetAnyRight();
		/*!
		* @brief Gets Any Up, be it from d-pad or joystick axes.
		* @return returns the pressed state of button/axes
		*/
		bool GetAnyUp();
		/*!
		* @brief Gets Any Down, be it from d-pad or joystick axes.
		* @return returns the pressed state of button/axes
		*/
		bool GetAnyDown();
		/*!
		 * @brief Goes through all registered joysticks and queries if a button has been pressed.
		 * @param button The button to query
		 * @return bool press state
		 */
		bool GetAnyJoystickButton(JoystickButtons button);
		/*!
		* @brief Goes through all registered joysticks and queries if a button has been pressed once.
		* @param button The button to query
		* @return bool press state
		*/
		bool GetAnyJoystickButtonOnce(JoystickButtons button);
		/*!
		* @brief Checks state of all joystick axis. see Frac::JoystickAxes
		* @param axis The axis to query. Can be Triggers
		* @return a floating point number ranging from -1.0 to 1.0
		*/
		float GetAnyJoystickAxis(JoystickAxes axis);
		/*!
		 * @brief Gets the current state of the mouse which gets updated in Update.
		 * @return const MouseState& which holds data of the mouse
		 */
		const MouseState& GetMouse() const { return m_mouse; }
		/*!
		* @brief Gets the current type of controller. Types defined in Frac::JoystickType.
		* @param joystick The joystick to get type of. See GetActiveJoysticks and GetDefaultJoystick
		* @return JoystickType
		*/
		JoystickType GetType(Joystick joystick) const;

	protected:
		virtual void AddJoystick(int joy) = 0;
		void RemoveJoystick(int joy);

		unsigned int							m_count{};
		std::map<int, JoystickState>			m_joyState;	/*!< Indexed by tokens */
		int										m_nextVirtual = 256;
		char									m_keyOnce[256 + 1]{};
		MouseState								m_mouse{};
	};

}
