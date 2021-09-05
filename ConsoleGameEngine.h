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
			return true;
		}

		virtual bool OnUserUpdate(float deltaTime) override
		{
			DrawTriangle({ 0, 0 }, { 0, 20 }, { 20, 20 }, PIXEL_SOLID, FG_BLUE);
			DrawLine({10, 10}, {20, 10}, PIXEL_SOLID, FG_RED);
			Draw({ 30, 10 }, PIXEL_SOLID, FG_DARK_GREEN);
			return true;
		}
	};

	int main()
	{
		Example demo;
		demo.Construct(120, 120, 4, 4);
	}
* Commands:
	SetTitle(title); -- set title of window, by default it's: "Undefined"
	Draw(pos1, PIXEL_TYPE, COLOUR); -- draws simple point
	DrawLine(pos1, pos2, PIXEL_TYPE, COLOUR); -- draw line
	DrawTriangle(pos1, pos2, pos3, PIXEL_TYPE, COLOUR); -- draw triangle
	DrawRectangle(pos, size, PIXEL_TYPE, COLOUR);
	GetWidth(); -- returns width of screen
	GetHeight(); -- returns height of screen
**/
#pragma endregion

#ifndef UNICODE
#error Please, enable UNICODE
#endif

#define Pixel wchar_t

#include <iostream>
#include <Windows.h>
#include <chrono>
#include <cmath>

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

struct vec2d
{
	int x;
	int y;
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

			rectWindow = { 0, 0, short(nScreenWidth - 1), short(nScreenHeight - 1) };
			SetConsoleWindowInfo(hConsole, TRUE, &rectWindow);

			screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			GameThread();

			return 0;
		}

	public:
		void SetTitle(std::wstring title);
		void Draw(vec2d pos, short c = 0x2588, short col = 0x000F);
		void DrawLine(vec2d pos1, vec2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawTriangle(vec2d pos1, vec2d pos2, vec2d pos3, short c = 0x2588, short col = 0x000F);
		void DrawRectangle(vec2d pos, vec2d size, short c = 0x2588, short col = 0x000F);
		void FillRectangle(vec2d pos, vec2d size, short c = 0x2588, short col = 0x000F);
		int GetWidth();
		int GetHeight();

	private:
		void GameThread()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;

			if (!OnUserCreate())
				exit(0);

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
		std::wstring sAppName = L"Undefined";
		int nScreenWidth = 120;
		int nScreenHeight = 40;
		int nFontW = 4;
		int nFontH = 4;
	};

	void ConsoleGameEngine::SetTitle(std::wstring title)
	{
		sAppName = title;
	}

	void ConsoleGameEngine::FillRectangle(vec2d pos, vec2d size, short c, short col)
	{
		if (pos.x >= 0 && pos.x < nScreenWidth && pos.y >= 0 && pos.y < nScreenHeight)
		{
			for (int i = pos.x; i < size.x; i++)
				for (int j = pos.y; j < size.y; j++)
				{
					screen[j * nScreenWidth + i].Char.UnicodeChar = c;
					screen[j * nScreenWidth + i].Attributes = col;
				}
		}
	}

	void ConsoleGameEngine::Draw(vec2d pos, short c, short col)
	{
		if (pos.x >= 0 && pos.x < nScreenWidth && pos.y >= 0 && pos.y < nScreenHeight)
		{
			screen[pos.y * nScreenWidth + pos.x].Char.UnicodeChar = c;
			screen[pos.y * nScreenWidth + pos.x].Attributes = col;
		}
	}

	void ConsoleGameEngine::DrawLine(vec2d pos1, vec2d pos2, short c, short col)
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

	void ConsoleGameEngine::DrawTriangle(vec2d pos1, vec2d pos2, vec2d pos3, short c, short col)
	{
		DrawLine(pos1, pos2, c, col);
		DrawLine(pos2, pos3, c, col);
		DrawLine(pos3, pos1, c, col);
	}

	void ConsoleGameEngine::DrawRectangle(vec2d pos, vec2d size, short c, short col)
	{
		DrawLine(pos, { size.x, pos.y });
		DrawLine({ size.x, pos.y }, size);
		DrawLine(size, { pos.x, size.y });
		DrawLine({ pos.x, size.y }, pos);
	}

	int ConsoleGameEngine::GetWidth()
	{
		return nScreenWidth;
	}

	int ConsoleGameEngine::GetHeight()
	{
		return nScreenHeight;
	}
}
