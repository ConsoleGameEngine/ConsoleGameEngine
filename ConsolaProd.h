#pragma once

#pragma region consolaprod_license
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

#pragma region consolaprod_sample
/**
* Example (engine only supports .spr files, check [this](https://github.com/defini7/SpriteEditor) for editing .spr files):
	#include "ConsolaProd.h"

	class Example : public def::ConsolaProd
	{
	public:
		Example()
		{
			sAppName = L"Example";
		}

	protected:
		bool OnUserCreate() override
		{
			return true;
		}

		bool OnUserUpdate(float fDeltaTime) override
		{
			for (int i = 0; i < GetScreenWidth(); i++)
				for (int j = 0; j < GetScreenHeight(); j++)
					Draw(i, j, def::PIXEL_SOLID, rand() % 15);

			return true;
		}
	};

	int main()
	{
			Example demo;
			def::rcode err = demo.ConstructConsole(120, 40, 12, 12);

			if (err.ok)
				demo.Run();
			else
				std::cerr << err.info << "\n";

			return 0;
	}
**/
#pragma endregion

#ifndef UNICODE
#error Enable Unicode in settings
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <thread>
#include <string>
#include <list>

#pragma comment(lib, "winmm.lib")

#ifdef XBOX_CONTROLLER
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")
#endif

namespace def
{
	enum FG_COLORS
	{
		FG_BLACK,
		FG_DARK_BLUE,
		FG_DARK_GREEN,
		FG_DARK_CYAN,
		FG_DARK_RED,
		FG_DARK_MAGENTA,
		FG_DARK_YELLOW,
		FG_GREY,
		FG_DARK_GREY,
		FG_BLUE,
		FG_GREEN,
		FG_CYAN,
		FG_RED,
		FG_MAGENTA,
		FG_YELLOW,
		FG_WHITE
	};

	enum BG_COLORS
	{
		BG_BLACK = 0x0000,
		BG_DARK_BLUE = 0x0010,
		BG_DARK_GREEN = 0x0020,
		BG_DARK_CYAN = 0x0030,
		BG_DARK_RED = 0x0040,
		BG_DARK_MAGENTA = 0x0050,
		BG_DARK_YELLOW = 0x0060,
		BG_GREY = 0x0070,
		BG_DARK_GREY = 0x0080,
		BG_BLUE = 0x0090,
		BG_GREEN = 0x00A0,
		BG_CYAN = 0x00B0,
		BG_RED = 0x00C0,
		BG_MAGENTA = 0x00D0,
		BG_YELLOW = 0x00E0,
		BG_WHITE = 0x00F0
	};

	enum PIXEL_TYPE
	{
		PIXEL_SOLID = 0x2588,
		PIXEL_THREEQUARTERS = 0x2593,
		PIXEL_HALF = 0x2592,
		PIXEL_QUARTER = 0x2591
	};

	struct rcode
	{
		bool ok;
		std::string info;
	};

	namespace Font
	{
		std::wstring CONSOLAS = L"Consolas";
		std::wstring COURIER_NEW = L"Courier New";
		std::wstring LUCIDA_CONSOLE = L"Lucida Console";
		std::wstring LUCIDA_SANS_TYPEWRITER = L"Lucida Sans Typewriter";
		std::wstring MS_GOTHIC = L"MS Gothic";
		std::wstring NSIMSUN = L"NSimSum";
		std::wstring SIMSUM_EXTB = L"SimSun-ExtB";
	}

	template <class T>
	class vec2d_basic
	{
	public:
		vec2d_basic()
		{
			this->x = 0;
			this->y = 0;
		}

		vec2d_basic(T x, T y)
		{
			this->x = x;
			this->y = y;
		}

		T x;
		T y;

		friend vec2d_basic<T> operator+(const vec2d_basic<T>& v1, const vec2d_basic<T>& v2)
		{
			return { v1.x + v2.x, v1.y + v2.y };
		}

		friend vec2d_basic<T> operator-(const vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			return { v1.x - v2.x, v1.y - v2.y };
		}

		friend vec2d_basic<T> operator*(const vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			return { v1.x * v2.x, v1.y * v2.y };
		}

		friend vec2d_basic<T> operator/(const vec2d_basic<T> v1, const vec2d_basic<T>& v2)
		{
			return { v1.x / v2.x, v1.y / v2.y };
		}

		friend vec2d_basic<T> operator+(const vec2d_basic<T> v1, const T v)
		{
			return { v1.x + v, v1.y + v };
		}

		friend vec2d_basic<T> operator-(const vec2d_basic<T> v1, const T v)
		{
			return { v1.x - v, v1.y - v };
		}

		friend vec2d_basic<T> operator*(const vec2d_basic<T> v1, const T v)
		{
			return { v1.x * v, v1.y * v };
		}

		friend vec2d_basic<T> operator/(const vec2d_basic<T> v1, const T v)
		{
			return { v1.x / v, v1.y / v };
		}

		float mag()
		{
			return sqrtf(this->x * this->x + this->y * this->y);
		}

		vec2d_basic<T> norm()
		{
			return { this->x / mag(), this->y / mag() };
		}

		// linear interpolation
		vec2d_basic<T> lerp(float x, float y, float t)
		{
			return { this->x + t * (x - this->x), this->y + t * (y - this->y) };
		}

		float dot(vec2d_basic<T> v, float angle)
		{
			return (this->x * v.x + this->y * v.y);
		}

		vec2d_basic<T> reflect()
		{
			return vec2d_basic<T>(-this->x, -this->y);
		}

		float distance()
		{
			return sqrtf(this->x * this->x + this->y * this->y);
		}
	};

	typedef vec2d_basic<float> vf2d;
	typedef vec2d_basic<int> vi2d;
	typedef vec2d_basic<double> vd2d;

	struct KeyState
	{
		bool bHeld;
		bool bReleased;
		bool bPressed;
	};

	const float PI = 2.0f * acos(0.0f);

#ifdef XBOX_CONTROLLER
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

			return dwConnection == ERROR_SUCCESS;
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

#endif

	class Sprite
	{
	public:
		Sprite()
		{
			Create(8, 8);
		}

		Sprite(int w, int h)
		{
			Create(w, h);
		}

		Sprite(std::wstring sFileName)
		{
			if (!Load(sFileName))
				Create(8, 8);
		}

	private:
		short* m_Glyphs = nullptr;
		short* m_Colours = nullptr;

	public:
		int nWidth = 0;
		int nHeight = 0;

	private:
		void Create(int w, int h)
		{
			nWidth = w;
			nHeight = h;

			m_Glyphs = new short[w * h];
			m_Colours = new short[w * h];

			for (int i = 0; i < w * h; i++)
			{
				m_Glyphs[i] = L' ';
				m_Colours[i] = FG_BLACK;
			}
		}

	public:
		void SetGlyph(vi2d pos, short c)
		{
			if (pos.x > 0 || pos.x < nWidth || pos.y > 0 || pos.y < nHeight)
				m_Glyphs[pos.y * nWidth + pos.x] = c;
		}

		void SetColour(vi2d pos, short c)
		{
			if (pos.x > 0 || pos.x < nWidth || pos.y > 0 || pos.y < nHeight)
				m_Colours[pos.y * nWidth + pos.x] = c;
		}

		short GetGlyph(vi2d pos)
		{
			if (pos.x > 0 || pos.x < nWidth || pos.y > 0 || pos.y < nHeight)
				return m_Glyphs[pos.y * nWidth + pos.x];

			return L' ';
		}

		short GetColour(vi2d pos)
		{
			if (pos.x > 0 || pos.x < nWidth || pos.y > 0 || pos.y < nHeight)
				return m_Colours[pos.y * nWidth + pos.x];

			return FG_BLACK;
		}

		bool Save(std::wstring sFile)
		{
			FILE* f = nullptr;

			_wfopen_s(&f, sFile.c_str(), L"wb");

			if (f == nullptr)
				return false;

			fwrite(&nWidth, sizeof(int), 1, f);
			fwrite(&nHeight, sizeof(int), 1, f);

			fwrite(m_Colours, sizeof(short), nWidth * nHeight, f);
			fwrite(m_Glyphs, sizeof(short), nWidth * nHeight, f);

			fclose(f);

			return true;
		}

		bool Load(std::wstring sFile)
		{
			delete[] m_Glyphs;
			delete[] m_Colours;

			nWidth = 0;
			nHeight = 0;

			FILE* f = nullptr;

			_wfopen_s(&f, sFile.c_str(), L"rb");

			if (f == nullptr)
				return false;

			std::fread(&nWidth, sizeof(int), 1, f);
			std::fread(&nHeight, sizeof(int), 1, f);

			Create(nWidth, nHeight);

			std::fread(m_Colours, sizeof(short), nWidth * nHeight, f);
			std::fread(m_Glyphs, sizeof(short), nWidth * nHeight, f);

			std::fclose(f);

			return true;
		}
	};

	class ConsolaProd
	{
	public:
		ConsolaProd()
		{
			hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

			hwnd = GetConsoleWindow();

			hDC = GetDC(hwnd);

			sAppName = L"Undefined";
			sFont = L"Consolas";
		}

		virtual ~ConsolaProd()
		{
			delete[] screen;
		}

	public:
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fDeltaTime) = 0;

		rcode ConstructConsole(int width = 120, int height = 40, int fontw = 4, int fonth = 4)
		{
			rcode rc;
			rc.ok = false;

			if (width <= 0 || height <= 0 || fontw <= 0 || fonth <= 0)
			{
				rc.info = "Invalid width or height";
				return rc;
			}

			nScreenWidth = width;
			nScreenHeight = height;

			nFontW = fontw;
			nFontH = fonth;

			hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

			if (hConsoleOut == INVALID_HANDLE_VALUE)
			{
				rc.info = "Can't create console screen buffer for output";
				return rc;
			}

			rectWindow = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);

			COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
			if (!SetConsoleScreenBufferSize(hConsoleOut, coord))
			{
				rc.info = "Too large or to small screen width or height";
				return rc;
			}

			if (!SetConsoleActiveScreenBuffer(hConsoleOut))
			{
				rc.info = "Can't set console screen buffer";
				return rc;
			}

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = nFontW;
			cfi.dwFontSize.Y = nFontH;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, sFont.c_str());
			if (!SetCurrentConsoleFontEx(hConsoleOut, false, &cfi))
			{
				rc.info = "Can't set font";
				return rc;
			}

			if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			{
				rc.info = "Could not set console mode (ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)";
				return rc;
			}

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
			{
				rc.info = "Could not get console screen buffer info";
				return rc;
			}

			if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
			{
				rc.info = "Specified screen height larger than maximum window height";
				return rc;
			}

			if (nScreenWidth > csbi.dwMaximumWindowSize.X)
			{
				rc.info = "Specified screen width larger than maximum window width";
				return rc;
			}

			rectWindow = { 0, 0, short(nScreenWidth - 1), short(nScreenHeight - 1) };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);
			screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			rc.ok = true;
			rc.info = "Ok";
			return rc;
		}

		void Run()
		{
			bGameThreadActive = true;

			std::thread t = std::thread(&def::ConsolaProd::AppThread, this);
			t.join();
		}

	public:
		virtual void Draw(vi2d pos, short c = 0x2588, short col = 0x000F);
		virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F);

		virtual void DrawLine(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		virtual void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);

		virtual void DrawTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		virtual void FillTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		virtual void DrawRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		virtual void DrawRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		virtual void DrawRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		virtual void DrawRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		virtual void FillRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		virtual void FillRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		virtual void FillRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		virtual void FillRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		virtual void DrawCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		virtual void DrawCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		virtual void FillCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		virtual void FillCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		virtual void DrawSprite(vi2d pos, Sprite* sprite);
		virtual void DrawSprite(int x, int y, Sprite* sprite);

		virtual void DrawPartialSprite(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite);
		virtual void DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);

		virtual void DrawPartialSpriteS(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite);
		virtual void DrawPartialSpriteS(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);

		virtual void DrawWireFrameModel(std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short c = 0x2588, short col = 0x000F);

		virtual void DrawString(vi2d pos, std::wstring text, short col = 0x000F);
		virtual void DrawString(int x, int y, std::wstring text, short col = 0x000F);

		void Clear(short c = 0x2588, short col = 0x000F);

		bool MakeSound(std::wstring sFilename, bool bLoop = false);
		bool Focused();

		inline vi2d GetMouse() const;
		inline int GetMouseX() const;
		inline int GetMouseY() const;

		inline KeyState GetMouse(short button) const;
		inline KeyState GetKey(short key) const;

		inline int GetCharacter(bool bHeld = true, bool bPressed = false, bool bReleased = false);

		inline int GetScreenWidth() const;
		inline int GetScreenHeight() const;
		inline vi2d GetScreenSize() const;

		inline float GetDeltaTime() const;

	private:
		void AppThread()
		{
			if (!OnUserCreate())
				bGameThreadActive = false;

			if (bGameThreadActive)
			{
				auto tp1 = std::chrono::system_clock::now();
				auto tp2 = std::chrono::system_clock::now();

				for (int i = 0; i < 256; i++)
					keys.push_back({ false, false, false });

				for (int i = 0; i < 5; i++)
					mouse.push_back({ false, false, false });

				while (bGameThreadActive)
				{
					tp2 = std::chrono::system_clock::now();
					std::chrono::duration<float> elapsedTime = tp2 - tp1;
					tp1 = tp2;
					fDeltaTime = elapsedTime.count();

					wchar_t buffer_title[256];
					swprintf_s(buffer_title, 256, L"github.com/defini7 - Consola Prod - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / fDeltaTime);
					SetConsoleTitleW(buffer_title);

					if (!OnUserUpdate(fDeltaTime))
						bGameThreadActive = false;

					INPUT_RECORD inBuf[32];
					DWORD events = 0;
					GetNumberOfConsoleInputEvents(hConsoleIn, &events);
					if (events > 0)
						ReadConsoleInputW(hConsoleIn, inBuf, events, &events);

					for (DWORD i = 0; i < events; i++)
					{
						switch (inBuf[i].EventType)
						{
						case FOCUS_EVENT:
							bFocused = inBuf[i].Event.FocusEvent.bSetFocus;
							break;

						case MOUSE_EVENT:
						{
							switch (inBuf[i].Event.MouseEvent.dwEventFlags)
							{
							case MOUSE_MOVED:
							{
								nMousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
								nMousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
							}
							break;

							case 0:
							{
								for (int m = 0; m < 5; m++)
									mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
							}
							break;

							default:
								break;
							}
						}
						break;

						default:
							break;
						}
					}

					for (int i = 0; i < 256; i++)
					{
						keyNewState[i] = GetAsyncKeyState(i);

						keys[i].bPressed = false;
						keys[i].bReleased = false;

						if (keyNewState[i] != keyOldState[i])
						{
							if (keyNewState[i] & 0x8000)
							{
								keys[i].bPressed = !keys[i].bHeld;
								keys[i].bHeld = true;
							}
							else
							{
								keys[i].bReleased = true;
								keys[i].bHeld = false;
							}
						}

						keyOldState[i] = keyNewState[i];
					}

					for (int m = 0; m < 5; m++)
					{
						mouse[m].bPressed = false;
						mouse[m].bReleased = false;

						if (mouseNewState[m] != mouseOldState[m])
						{
							if (mouseNewState[m])
							{
								mouse[m].bPressed = true;
								mouse[m].bHeld = true;
							}
							else
							{
								mouse[m].bReleased = true;
								mouse[m].bHeld = false;
							}
						}

						mouseOldState[m] = mouseNewState[m];
					}

					WriteConsoleOutputW(hConsoleOut, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &rectWindow);
				}
			}
		}

	protected:
		std::wstring sAppName;
		std::wstring sFont;

	private:
		CHAR_INFO* screen = nullptr;
		HANDLE hConsoleOut;
		HANDLE hConsoleIn;
		SMALL_RECT rectWindow;
		HWND hwnd;
		HDC hDC;

		std::vector<KeyState> keys;
		std::vector<KeyState> mouse;

		short keyOldState[256]{ 0 };
		short keyNewState[256]{ 0 };

		bool mouseOldState[5] = { 0 };
		bool mouseNewState[5] = { 0 };

		int nMousePosX;
		int nMousePosY;

		int nScreenWidth;
		int nScreenHeight;

		int nFontW;
		int nFontH;

		float fDeltaTime;

		bool bGameThreadActive;
		bool bFocused;
	};

	bool ConsolaProd::MakeSound(std::wstring sFilename, bool bLoop)
	{
		DWORD f = SND_ASYNC | SND_FILENAME;

		if (bLoop)
			f |= SND_LOOP;

		return (bool)PlaySoundW(sFilename.c_str(), nullptr, f);
	}

	bool ConsolaProd::Focused()
	{
		return bFocused;
	}

	void ConsolaProd::FillRectangle(vi2d pos1, vi2d pos2, short c, short col)
	{
		for (int i = pos1.x; i < pos2.x; i++)
			for (int j = pos1.y; j < pos2.y; j++)
			{
				screen[j * nScreenWidth + i].Char.UnicodeChar = c;
				screen[j * nScreenWidth + i].Attributes = col;
			}
	}

	void ConsolaProd::FillRectangle(int x1, int y1, int x2, int y2, short c, short col)
	{
		FillRectangle({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsolaProd::FillRectangleS(vi2d pos, vi2d size, short c, short col)
	{
		FillRectangle(pos, { pos.x + size.x, pos.y + size.y }, c, col);
	}

	void ConsolaProd::FillRectangleS(int x, int y, int size_x, int size_y, short c, short col)
	{
		FillRectangle({ x, y }, { x + size_x, y + size_y }, c, col);
	}

	void ConsolaProd::DrawCircle(vi2d pos, int radius, short c, short col)
	{
		if (!radius) return;

		int x = 0;
		int y = radius;
		int p = 3 - 2 * radius;

		while (y >= x)
		{
			Draw({ pos.x - x, pos.y - y }, c, col);	// upper left left
			Draw({ pos.x - y, pos.y - x }, c, col);	// upper upper left
			Draw({ pos.x + y, pos.y - x }, c, col);	// upper upper right
			Draw({ pos.x + x, pos.y - y }, c, col);	// upper right right
			Draw({ pos.x - x, pos.y + y }, c, col);	// lower left left
			Draw({ pos.x - y, pos.y + x }, c, col);	// lower lower left
			Draw({ pos.x + y, pos.y + x }, c, col);	// lower lower right
			Draw({ pos.x + x, pos.y + y }, c, col);	// lower right right
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void ConsolaProd::DrawCircle(int x, int y, int radius, short c, short col)
	{
		DrawCircle({ x, y }, radius, c, col);
	}

	void ConsolaProd::FillCircle(vi2d pos, int radius, short c, short col)
	{
		if (!radius) return;

		int x = 0;
		int y = radius;
		int p = 3 - 2 * radius;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y >= x)
		{
			drawline(pos.x - x, pos.x + x, pos.y - y);
			drawline(pos.x - y, pos.x + y, pos.y - x);
			drawline(pos.x - x, pos.x + x, pos.y + y);
			drawline(pos.x - y, pos.x + y, pos.y + x);
			if (p < 0) p += 4 * x++ + 6;
			else p += 4 * (x++ - y--) + 10;
		}
	}

	void ConsolaProd::FillCircle(int x, int y, int radius, short c, short col)
	{
		FillCircle({ x, y }, radius, c, col);
	}

	void ConsolaProd::Draw(vi2d pos, short c, short col)
	{
		if (pos.x >= 0 && pos.x <= nScreenWidth && pos.y >= 0 && pos.y <= nScreenHeight)
		{
			screen[pos.y * nScreenWidth + pos.x].Char.UnicodeChar = c;
			screen[pos.y * nScreenWidth + pos.x].Attributes = col;
		}
	}

	void ConsolaProd::Draw(int x, int y, short c, short col)
	{
		Draw({ x, y }, c, col);
	}

	void ConsolaProd::DrawLine(vi2d pos1, vi2d pos2, short c, short col)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = pos2.x - pos1.x; dy = pos2.y - pos1.y;
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = pos1.x; y = pos1.y; xe = pos2.x;
			}
			else
			{
				x = pos2.x; y = pos2.y; xe = pos1.x;
			}

			Draw({ x, y }, c, col);

			for (i = 0; x < xe; i++)
			{
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				Draw({ x, y }, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = pos1.x; y = pos1.y; ye = pos2.y;
			}
			else
			{
				x = pos2.x; y = pos2.y; ye = pos1.y;
			}

			Draw({ x, y }, c, col);

			for (i = 0; y < ye; i++)
			{
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				Draw({ x, y }, c, col);
			}
		}
	}

	void ConsolaProd::DrawLine(int x1, int y1, int x2, int y2, short c, short col)
	{
		DrawLine({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsolaProd::DrawTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c, short col)
	{
		DrawLine(pos1, pos2, c, col);
		DrawLine(pos2, pos3, c, col);
		DrawLine(pos3, pos1, c, col);
	}

	void ConsolaProd::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		DrawTriangle({ x1, y1 }, { x2, y2 }, { x3, y3 }, c, col);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void ConsolaProd::FillTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c, short col)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw({ i, ny }, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (pos1.y > pos2.y) { std::swap(pos1.y, pos2.y); std::swap(pos1.x, pos2.x); }
		if (pos1.y > pos3.y) { std::swap(pos1.y, pos3.y); std::swap(pos1.x, pos3.x); }
		if (pos2.y > pos3.y) { std::swap(pos2.y, pos3.y); std::swap(pos2.x, pos3.x); }

		t1x = t2x = pos1.x; y = pos1.y;   // Starting points
		dx1 = (int)(pos2.x - pos1.x); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(pos2.y - pos1.y);

		dx2 = (int)(pos3.x - pos1.x); if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(pos3.y - pos1.y);

		if (dy1 > dx1) {   // swap values
			std::swap(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2) {   // swap values
			std::swap(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (pos1.y == pos2.y) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
											// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == pos2.y) break;

		}
	next:
		// Second half
		dx1 = (int)(pos3.x - pos2.x); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(pos3.y - pos2.y);
		t1x = pos2.x;

		if (dy1 > dx1) {   // swap values
			std::swap(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != pos3.x) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x; if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x; if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > pos3.y) return;
		}
	}

	void ConsolaProd::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		FillTriangle({ x1, y1 }, { x2, y2 }, { x3, y3 }, c, col);
	}

	void ConsolaProd::DrawRectangle(vi2d pos1, vi2d pos2, short c, short col)
	{
		for (int x = pos1.x; x <= pos2.x; x++)
		{
			Draw(pos1.x + x, pos1.y, c, col);
			Draw(pos1.x + x, pos2.y, c, col);
		}

		for (int y = pos1.y; y <= pos2.y; y++)
		{
			Draw(pos1.x, pos1.y + y, c, col);
			Draw(pos2.x, pos1.y + y, c, col);
		}
	}

	void ConsolaProd::DrawRectangle(int x1, int y1, int x2, int y2, short c, short col)
	{
		DrawRectangle({ x1, y1 }, { x2, y2 }, c, col);
	}

	void ConsolaProd::DrawRectangleS(vi2d pos, vi2d size, short c, short col)
	{
		DrawRectangle(pos, { pos.x + size.x, pos.y + size.y }, c, col);
	}

	void ConsolaProd::DrawRectangleS(int x, int y, int size_x, int size_y, short c, short col)
	{
		DrawRectangle({ x, y }, { x + size_x, y + size_y }, c, col);
	}

	void ConsolaProd::DrawSprite(vi2d pos, Sprite* sprite)
	{
		if (sprite == nullptr)
			return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph({ i, j }) != L' ')
					Draw({ pos.x + i, pos.y + j }, sprite->GetGlyph({ i, j }), sprite->GetColour({ i, j }));
			}
		}
	}

	void ConsolaProd::DrawSprite(int x, int y, Sprite* sprite)
	{
		DrawSprite({ x, y }, sprite);
	}

	void ConsolaProd::DrawPartialSprite(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite)
	{
		if (sprite == nullptr || fpos1.x < 0 || fpos1.y < 0 || fpos2.x > sprite->nWidth || fpos2.y > sprite->nHeight)
			return;

		for (int i = fpos1.x, x = 0; i < fpos2.x; i++, x++)
		{
			for (int j = fpos1.y, y = 0; j < fpos2.y; j++, y++)
			{
				if (sprite->GetGlyph({ i, j }) != L' ')
					Draw({ pos.x + x, pos.y + y }, sprite->GetGlyph({ i, j }), sprite->GetColour({ i, j }));
			}
		}
	}

	void ConsolaProd::DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite)
	{
		DrawPartialSprite({ x, y }, { fx1, fy1 }, { fx2, fy2 }, sprite);
	}

	void ConsolaProd::DrawPartialSpriteS(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite)
	{
		DrawPartialSprite({ pos.x, pos.y }, { fpos1.x, fpos1.y }, { fpos1.x + fpos2.x, fpos1.y + fpos2.y }, sprite);
	}

	void ConsolaProd::DrawPartialSpriteS(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite)
	{
		DrawPartialSprite({ x, y }, { fx1, fy1 }, { fx1 + fx2, fy1 + fy2 }, sprite);
	}

	void ConsolaProd::DrawWireFrameModel(std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r, float s, short c, short col)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, c, col);
		}
	}

	void ConsolaProd::DrawString(vi2d pos, std::wstring text, short col)
	{
		for (size_t i = 0; i < text.size(); i++)
		{
			screen[pos.y * nScreenWidth + pos.x + i].Char.UnicodeChar = text[i];
			screen[pos.y * nScreenWidth + pos.x + i].Attributes = col;
		}
	}

	void ConsolaProd::DrawString(int x, int y, std::wstring text, short col)
	{
		DrawString({ x, y }, text, col);
	}

	void ConsolaProd::Clear(short c, short col)
	{
		FillRectangle({ 0, 0 }, { nScreenWidth, nScreenHeight }, c, col);
	}

	inline vi2d ConsolaProd::GetMouse() const
	{
		return { nMousePosX, nMousePosY };
	}

	inline int ConsolaProd::GetMouseX() const
	{
		return nMousePosX;
	}

	inline int ConsolaProd::GetMouseY() const
	{
		return nMousePosY;
	}

	inline KeyState ConsolaProd::GetMouse(short button) const
	{
		return mouse[button];
	}

	inline KeyState ConsolaProd::GetKey(short key) const
	{
		return keys[key];
	}

	inline int ConsolaProd::GetScreenWidth() const
	{
		return nScreenWidth;
	}

	inline int ConsolaProd::GetScreenHeight() const
	{
		return nScreenHeight;
	}

	inline int ConsolaProd::GetCharacter(bool bHeld, bool bPressed, bool bReleased)
	{
		for (int i = ' '; i <= '~'; i++)
		{
			if (bHeld)
			{
				if (keys[i].bHeld)
					return i;
			}
			else if (bPressed)
			{
				if (keys[i].bPressed)
					return i;
			}
			else if (bReleased)
			{
				if (keys[i].bReleased)
					return i;
			}

		}

		return -1;
	}

	inline vi2d ConsolaProd::GetScreenSize() const
	{
		return { nScreenWidth, nScreenHeight };
	}

	inline float ConsolaProd::GetDeltaTime() const
	{
		return fDeltaTime;
	}
}
