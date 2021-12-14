#pragma once

#pragma region LICENSE
/***
*	BSD 3-Clause License

	Copyright (c) 2021, Alex
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	   list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	   this list of conditions and the following disclaimer in the documentation
	   and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	   contributors may be used to endorse or promote products derived from
	   this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***/
#pragma endregion

#include <Xinput.h>
#include <Windows.h>

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

		BYTE GetJoy(short key)
		{
			return GetState().Gamepad.wButtons & key;
		}
	};
}
