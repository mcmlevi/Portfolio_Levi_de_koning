#pragma once
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <Input/IController.h>


namespace Th
{
	class WinController : public IController
	{
	public:
		WinController();
		virtual ~WinController() = default;
		void UpdateStatus() override;
		void InitializeController() override;

		BOOL EnumerateGameControllers(LPCDIDEVICEINSTANCE devIstance);
	private:
		void initializeGameController();

		std::vector<LPDIRECTINPUTDEVICE8> m_availableDevices{};
		int m_activeController{};


	};
}


