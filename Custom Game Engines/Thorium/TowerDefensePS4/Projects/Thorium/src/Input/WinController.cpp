#include "Thpch.h"
#include <Input/WinController.h>
#include <Graphics/Window/Win32Window.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


namespace Th
{
	static IDirectInput8* gDirectInputInterface = nullptr;

	const int winDpadUp = 0;
	const int winDpadRight = 9000;
	const int winDpadDown = 18000;
	const int winDpadLeft = 27000;
	const int winDpadUpRight = 4500;
	const int winDpadRightDown = 13500;
	const int winDpadBottonLeft = 22500;
	const int winDpadLeftUp = 31500;


	BOOL DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
	{
		WinController* controller = (WinController*)pvRef;
		return controller->EnumerateGameControllers(lpddi);
	}
	// set controller axes
	BOOL CALLBACK staticSetGameControllerAxesRanges(LPCDIDEVICEOBJECTINSTANCE devObjInst, LPVOID pvRef)
	{
		// the game controller
		LPDIRECTINPUTDEVICE8 gameController = (LPDIRECTINPUTDEVICE8)pvRef;
		gameController->Unacquire();

		// structure to hold game controller range properties
		DIPROPRANGE gameControllerRange;

		// set the range to -100 and 100
		gameControllerRange.lMin = -100;
		gameControllerRange.lMax = 100;

		// set the size of the structure
		gameControllerRange.diph.dwSize = sizeof(DIPROPRANGE);
		gameControllerRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);

		// set the object that we want to change		
		gameControllerRange.diph.dwHow = DIPH_BYID;
		gameControllerRange.diph.dwObj = devObjInst->dwType;

		// now set the range for the axis		
		if (FAILED(gameController->SetProperty(DIPROP_RANGE, &gameControllerRange.diph))) {
			return DIENUM_STOP;
		}



		// structure to hold game controller axis dead zone
		DIPROPDWORD gameControllerDeadZone;

		// set the dead zone to 1%
		gameControllerDeadZone.dwData = 100;

		// set the size of the structure
		gameControllerDeadZone.diph.dwSize = sizeof(DIPROPDWORD);
		gameControllerDeadZone.diph.dwHeaderSize = sizeof(DIPROPHEADER);

		// set the object that we want to change
		gameControllerDeadZone.diph.dwHow = DIPH_BYID;
		gameControllerDeadZone.diph.dwObj = devObjInst->dwType;

		// now set the dead zone for the axis
		if (FAILED(gameController->SetProperty(DIPROP_DEADZONE, &gameControllerDeadZone.diph)))
			return DIENUM_STOP;

		return DIENUM_CONTINUE;
	}
	WinController::WinController() :IController{}
	{
	}

	void WinController::UpdateStatus()
	{
		HRESULT hr;
		if (m_activeController >= m_availableDevices.size())
			return;
		LPDIRECTINPUTDEVICE8 device = m_availableDevices[m_activeController];
		// poll the device to read the current state
		hr = m_availableDevices[m_activeController]->Poll();

		if (FAILED(hr))
		{
			// DirectInput lost the device, try to re-acquire it
			hr = device->Acquire();
			while (hr == DIERR_INPUTLOST)
				hr = device->Acquire();

			// return if a fatal error is encountered
			if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED))
			{
				printf("Something very bad happened with polling the controller input \n");
				return;
			}

			// if another application has control of this device, we have to wait for our turn
			if (hr == DIERR_OTHERAPPHASPRIO)
				return;
		}
		DIJOYSTATE state{};
		// now if everything is okay, we can get the state of the device
		if (FAILED(hr = device->GetDeviceState(sizeof(DIJOYSTATE), &state)))
		{
			printf("Failed to get the state\n");
		}

		m_controllerState = Th::ControllerState{};

		m_controllerState.LStick[0] = static_cast<float>(state.lX) / 100.f;
		m_controllerState.LStick[1] = static_cast<float>(state.lY) / 100.f;
		m_controllerState.RStick[0] = static_cast<float>(state.lZ) / 100.f;
		m_controllerState.RStick[1] = static_cast<float>(state.lRz) / 100.f;


		switch (state.rgdwPOV[0])
		{
		case winDpadUp:
			m_controllerState.DpadState |= Th::DpadFlags::DpadUp;
			break;
		case winDpadRight:
			m_controllerState.DpadState |= Th::DpadFlags::DpadRight;
			break;
		case winDpadDown:
			m_controllerState.DpadState |= Th::DpadFlags::DpadDown;
			break;
		case winDpadLeft:
			m_controllerState.DpadState |= Th::DpadFlags::DpadLeft;
			break;
		case winDpadUpRight:
			m_controllerState.DpadState |= Th::DpadFlags::DpadUp;
			m_controllerState.DpadState |= Th::DpadFlags::DpadRight;
			break;
		case winDpadRightDown:
			m_controllerState.DpadState |= Th::DpadFlags::DpadRight;
			m_controllerState.DpadState |= Th::DpadFlags::DpadDown;
			break;
		case winDpadBottonLeft:
			m_controllerState.DpadState |= Th::DpadFlags::DpadDown;
			m_controllerState.DpadState |= Th::DpadFlags::DpadLeft;
			break;
		case winDpadLeftUp:
			m_controllerState.DpadState |= Th::DpadFlags::DpadLeft;
			m_controllerState.DpadState |= Th::DpadFlags::DpadUp;
			break;
		default:
			m_controllerState.DpadState = 0;
			break;
		}


		if (state.rgbButtons[0])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnLeft;
		}
		if (state.rgbButtons[1])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnDown;
		}
		if (state.rgbButtons[2])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnRight;
		}
		if (state.rgbButtons[3])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnUp;
		}
		if (state.rgbButtons[4])
		{
			m_controllerState.LShoulderButton = 1;
		}
		if (state.rgbButtons[5])
		{
			m_controllerState.RShoulderButton = 1;
		}

		if (state.rgbButtons[8])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnShare;
		}
		if (state.rgbButtons[9])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnOptions;
		}

		if (state.rgbButtons[10])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnL3;
		}
		if (state.rgbButtons[11])
		{
			m_controllerState.ControllerBtnState |= Th::BtnFlags::BtnR3;
		}
		if (state.rgbButtons[12])
		{
			m_controllerState.MenueBtn = 1;
		}

		m_controllerState.LTrigger = 1.f - static_cast<float>(abs(state.lRx)) / 100.f;
		m_controllerState.RTrigger = 1.f - static_cast<float>(abs(state.lRy)) / 100.f;



	}

	void WinController::InitializeController()
	{
		if (gDirectInputInterface == nullptr)
		{
			if (FAILED(DirectInput8Create((HINSTANCE)GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&gDirectInputInterface, NULL)))
			{
				printf("Failed to create direct input interface\n");
			}
		}

		if (FAILED(gDirectInputInterface->EnumDevices(DI8DEVCLASS_GAMECTRL, &DIEnumDevicesCallback, (void*)this, DIEDFL_ATTACHEDONLY)))
		{
			printf("No Devices Fount\n");
		}

		initializeGameController();
	}

	BOOL WinController::EnumerateGameControllers(LPCDIDEVICEINSTANCE devIstance)
	{
		LPDIRECTINPUTDEVICE8 gameController;

		// create interface for the current game controller
		if (FAILED(gDirectInputInterface->CreateDevice(devIstance->guidInstance, &gameController, NULL)))
		{
			return DIENUM_CONTINUE;
		}
		else
		{
			// store the game controller
			m_availableDevices.push_back(gameController);
			return DIENUM_CONTINUE;
		}
	}

	void WinController::initializeGameController()
	{
		assert(m_activeController < m_availableDevices.size());
		// get currently active game controller
		LPDIRECTINPUTDEVICE8 gameController = m_availableDevices[m_activeController];
		// set data format
		if (FAILED(gameController->SetDataFormat(&c_dfDIJoystick)))
		{
			printf("Failed to set data format of the joystick\n");

		}

		// set range and dead zone of joystick axes
		if (FAILED(gameController->EnumObjects(&staticSetGameControllerAxesRanges, gameController, DIDFT_AXIS)))
		{
			printf("Unable to set deadzones for the controller  \n");
		}

		// acquire the controller
		if (FAILED(gameController->Acquire()))
		{
			printf("unable to aquire the deadzone of the controller \n");
		}
	}


}

