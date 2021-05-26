#pragma once

#include <cstdint>

namespace Th
{
	namespace DpadFlags
	{
		const std::uint8_t DpadUp{ 1 << 0 };
		const std::uint8_t DpadRight{ 1 << 1 };
		const std::uint8_t DpadDown{ 1 << 2 };
		const std::uint8_t DpadLeft{ 1 << 3 };
	}

	namespace BtnFlags
	{
		const std::uint8_t BtnUp{ 1 << 0 };
		const std::uint8_t BtnRight{ 1 << 1 };
		const std::uint8_t BtnDown{ 1 << 2 };
		const std::uint8_t BtnLeft{ 1 << 3 };
		const std::uint8_t BtnL3{ 1 << 4 };
		const std::uint8_t BtnR3{ 1 << 5 };
		const std::uint8_t BtnShare{ 1 << 6 };
		const std::uint8_t BtnOptions{ 1 << 7 };
		
	}

	struct ControllerState
	{
		float LStick[2]{};
		float RStick[2]{};

		float LTrigger{};
		int LShoulderButton{};
		float RTrigger{};
		int RShoulderButton{};

		// state of the Dpad 
		std::uint8_t DpadState{};
		//  State of the other buttons on the controller
		std::uint8_t ControllerBtnState{};

		bool Connected{ false };
		int MenueBtn{};
	};
}
