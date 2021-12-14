#pragma once

#include <Xinput.h>
#pragma comment(lib, "XInput.lib")

namespace def
{
	class XBOX_Controller
	{
	public:
		XINPUT_STATE m_ControllerState;
		int nControllerID;

		XBOX_Controller(int nPlayer)
		{
			this->nControllerID = nPlayer - 1;
		}

		XINPUT_STATE GetState()
		{
			ZeroMemory(&this->m_ControllerState, sizeof(XINPUT_STATE));
			XInputGetState(this->nControllerID, &this->m_ControllerState);

			return this->m_ControllerState;
		}

		bool IsConnected()
		{
			ZeroMemory(&this->m_ControllerState, sizeof(XINPUT_STATE));
			DWORD dwConnection = XInputGetState(this->nControllerID, &this->m_ControllerState);

			return (dwConnection == ERROR_SUCCESS) ? true : false;
		}

		void SetVibration(int nLeftValue, int nRightValue)
		{
			XINPUT_VIBRATION m_Vibration;
			ZeroMemory(&m_Vibration, sizeof(XINPUT_VIBRATION));

			m_Vibration.wLeftMotorSpeed = nLeftValue;
			m_Vibration.wRightMotorSpeed = nRightValue;

			XInputSetState(nControllerID, &m_Vibration);
		}

		BYTE GetButton(short key)
		{
			return GetState().Gamepad.wButtons & key;
		}
	};
}
