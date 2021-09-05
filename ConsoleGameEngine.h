#pragma once

#pragma region LICENSE
/***
*	BSD 3-Clause License

	Copyright (c) 2021, deffy
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

#pragma region DOCS
/**
* Example:
	#include "ConsoleGameEngine.h"

	class Example : public cge::ConsoleGameEngine
	{
	public:
		virtual bool OnUserCreate() override
		{
			col = FG_RED;
			return true;
		}

		virtual bool OnUserUpdate(float deltaTime) override
		{
			for (int i = 0; i < GetWidth(); i++)
				for (int j = 0; j < GetHeight(); j++)
				{
					Draw(i, j, PIXEL_SOLID, col);
				}
			return true;
		}
	
	private:
		Pixel col;
	};

	int main()
	{
		Example demo;
		demo.Construct(120, 120, 4, 4);
	}
* Commands:
	Draw(x, y, PIXEL_TYPE, COLOUR);
**/
#pragma endregion

#ifndef UNICODE
#error Please, enable UNICODE
#endif

#define Pixel wchar_t

#include <iostream>
#include <Windows.h>
#include <chrono>

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007,
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
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
	BG_WHITE = 0x00F0,
};

enum PIXEL_TYPE
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591,
};

namespace cge
{
	class ConsoleGameEngine
	{
	public:
		virtual ~ConsoleGameEngine()
		{
			delete[] screen;
		}

	public:
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fDeltaTime) = 0;

		int Construct(int width, int height, int fontw, int fonth)
		{
			nScreenWidth = width;
			nScreenHeight = height;

			nFontW = fontw;
			nFontH = fonth;

			sAppName = L"Default";

			hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

			if (hConsole == INVALID_HANDLE_VALUE)
				exit(-1);

			rectWindow = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

			COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
			if (!SetConsoleScreenBufferSize(hConsole, coord))
				exit(-1);

			if (!SetConsoleActiveScreenBuffer(hConsole))
				exit(-2);

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = nFontW;
			cfi.dwFontSize.Y = nFontH;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, L"Consolas");
			if (!SetCurrentConsoleFontEx(hConsole, false, &cfi))
				exit(-3);

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
				exit(-4);
			if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
				exit(-5);
			if (nScreenWidth > csbi.dwMaximumWindowSize.X)
				exit(-5);

			rectWindow = { 0, 0, static_cast<short>(nScreenWidth), static_cast<short>(nScreenHeight) };
			SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

			screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			GameThread();

			return 0;
		}

	public:
		void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
		{
			if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
			{
				screen[y * nScreenWidth + x].Char.UnicodeChar = c;
				screen[y * nScreenWidth + x].Attributes = col;
			}
		}

		int GetWidth()
		{
			return nScreenWidth;
		}

		int GetHeight()
		{
			return nScreenHeight;
		}

	private:
		void GameThread()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;

			if (OnUserCreate() == 0)
			{
				std::cerr << "Please override OnUserCreate!" << std::endl;
				exit(0);
			}

			if (OnUserUpdate(elapsedTime.count()) == 0)
			{
				std::cerr << "Please override OnUserUpdate!" << std::endl;
				exit(0);
			}

			if (!OnUserCreate())
			{
				exit(0);
			}

			while (1)
			{
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float deltaTime = elapsedTime.count();

				wchar_t buffer_title[256];
				swprintf_s(buffer_title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / deltaTime);
				SetConsoleTitle(buffer_title);

				if (!OnUserUpdate(deltaTime))
					break;

				WriteConsoleOutput(hConsole, screen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &rectWindow);
			}
		}

	protected:
		CHAR_INFO* screen = nullptr;
		HANDLE hConsole;
		SMALL_RECT rectWindow;
		std::wstring sAppName = L"Default";
		int nScreenWidth = 120;
		int nScreenHeight = 40;
		int nFontW = 4;
		int nFontH = 4;
	};
}
