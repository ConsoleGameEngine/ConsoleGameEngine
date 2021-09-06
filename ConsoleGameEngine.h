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
* Example (engine only supports .spr files, check https://github.com/defini7/lab/tree/main/Sprite_Editor for editing .spr files):
	#include "ConsoleGameEngine.h"

	class Example : public cge::ConsoleGameEngine
	{
	public:
		virtual bool OnUserCreate() override
		{
			spr = new cge::Sprite(L"ball.png");
			return true;
		}

		virtual bool OnUserUpdate(float deltaTime) override
		{
			DrawTriangle({ 0, 0 }, { 0, 20 }, { 20, 20 }, PIXEL_SOLID, FG_BLUE);
			DrawLine({10, 10}, {20, 10}, PIXEL_SOLID, FG_RED);
			Draw({ 30, 10 }, PIXEL_SOLID, FG_DARK_GREEN);
			return true;
		}

	private:
		cge::Sprite* spr = nullptr;

	};

	int main()
	{
		Example demo;
		demo.Construct(120, 120, 4, 4);
	}
* Commands:
	SetTitle(title); -- set title of window, by default it's: "Undefined"
	Draw(pos1, PIXEL_TYPE, COLOUR); -- draws simple point
	DrawSprite(sprite); -- draws sprite
	DrawLine(pos1, pos2, PIXEL_TYPE, COLOUR); -- draw line
	DrawTriangle(pos1, pos2, pos3, PIXEL_TYPE, COLOUR); -- draw triangle
	DrawRectangle(pos, size, PIXEL_TYPE, COLOUR); -- draws rectangle
	FillRectangle(pos, size, PIXEL_TYPE, COLOUR); -- draws and fill rectangle
	GetWidth(); -- returns width of screen
	GetHeight(); -- returns height of screen
**/
#pragma endregion

#ifndef UNICODE
#error Please, enable UNICODE
#endif

#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <cmath>

namespace cge
{
	namespace Colour
	{
		namespace FG
		{
			int BLACK = 0x0000,
				DARK_BLUE = 0x0001,
				DARK_GREEN = 0x0002,
				DARK_CYAN = 0x0003,
				DARK_RED = 0x0004,
				DARK_MAGENTA = 0x0005,
				DARK_YELLOW = 0x0006,
				GREY = 0x0007,
				DARK_GREY = 0x0008,
				BLUE = 0x0009,
				GREEN = 0x000A,
				CYAN = 0x000B,
				RED = 0x000C,
				MAGENTA = 0x000D,
				YELLOW = 0x000E,
				WHITE = 0x000F;
		}
		namespace BG
		{
			int BLACK = 0x0000,
				DARK_BLUE = 0x0010,
				DARK_GREEN = 0x0020,
				DARK_CYAN = 0x0030,
				DARK_RED = 0x0040,
				DARK_MAGENTA = 0x0050,
				DARK_YELLOW = 0x0060,
				GREY = 0x0070,
				DARK_GREY = 0x0080,
				BLUE = 0x0090,
				GREEN = 0x00A0,
				CYAN = 0x00B0,
				RED = 0x00C0,
				MAGENTA = 0x00D0,
				YELLOW = 0x00E0,
				WHITE = 0x00F0;
		}
	}

	namespace Pixel
	{
		int SOLID = 0x2588,
			THREEQUARTERS = 0x2593,
			HALF = 0x2592,
			QUARTER = 0x2591;
	}

	struct vec2d
	{
		int x;
		int y;
	};

	struct KeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	};

	class Sprite
	{
	public:
		Sprite()
		{

		}

		Sprite(int w, int h)
		{
			Create(w, h);
		}

		Sprite(std::wstring sFile)
		{
			if (!Load(sFile))
				Create(8, 8);
		}

		int nWidth = 0;
		int nHeight = 0;

	private:
		short* m_Glyphs = nullptr;
		short* m_Colours = nullptr;

		void Create(int w, int h)
		{
			nWidth = w;
			nHeight = h;
			m_Glyphs = new short[w * h];
			m_Colours = new short[w * h];
			for (int i = 0; i < w * h; i++)
			{
				m_Glyphs[i] = L' ';
				m_Colours[i] = Colour::FG::BLACK;
			}
		}

	public:
		void SetGlyph(vec2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Glyphs[pos.y * nWidth + pos.x] = c;
		}

		void SetColour(vec2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Colours[pos.y * nWidth + pos.x] = c;
		}

		short GetGlyph(vec2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return L' ';
			else
				return m_Glyphs[pos.y * nWidth + pos.x];
		}

		short GetColour(vec2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return Colour::FG::BLACK;
			else
				return m_Colours[pos.y * nWidth + pos.x];
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

			AppThread();

			return 0;
		}

		std::vector<KeyState> keys;

	public:
		void SetTitle(std::wstring title);
		void Draw(vec2d pos, short c = 0x2588, short col = 0x000F);
		void DrawLine(vec2d pos1, vec2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawTriangle(vec2d pos1, vec2d pos2, vec2d pos3, short c = 0x2588, short col = 0x000F);
		void FillTriangle(vec2d pos1, vec2d pos2, vec2d pos3, short c = 0x2588, short col = 0x000F);
		void DrawRectangle(vec2d pos1, vec2d pos2, short c = 0x2588, short col = 0x000F);
		void FillRectangle(vec2d pos1, vec2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawSprite(vec2d pos, Sprite* sprite);
		int GetWidth();
		int GetHeight();

	private:
		void AppThread()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;

			if (!OnUserCreate())
				exit(0);

			for (int i = 0; i < 256; i++)
			{
				keys.push_back({false, false, false});
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

				for (int i = 0; i < 256; i++)
				{
					short keyState = GetKeyState(i);
					if (keyState & 0x8000)
					{
						keys[i].bHeld = true;
						keys[i].bPressed = false;
						keys[i].bReleased = false;
					}
					else if (keyState & 1)
					{
						keys[i].bPressed = true;
						keys[i].bHeld = false;
						keys[i].bReleased = false;
					}
					else
					{
						keys[i].bReleased = true;
						keys[i].bHeld = false;
						keys[i].bPressed = false;
					}
						
				}

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

	void ConsoleGameEngine::FillRectangle(vec2d pos1, vec2d pos2, short c, short col)
	{
		if (pos1.x >= 0 && pos1.x < nScreenWidth && pos1.y >= 0 && pos1.y < nScreenHeight)
		{
			for (int i = pos1.x; i < pos2.x; i++)
				for (int j = pos1.y; j < pos2.y; j++)
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

	void ConsoleGameEngine::FillTriangle(vec2d pos1, vec2d pos2, vec2d pos3, short c, short col)
	{
		DrawTriangle(pos1, pos2, pos3, c, col);
		for (int i = 0; i < 10; i++)
		{
			DrawTriangle({ pos1.x + i, pos1.y + i }, pos2, pos3, c, col);
		}
	}

	void ConsoleGameEngine::DrawRectangle(vec2d pos1, vec2d pos2, short c, short col)
	{
		DrawLine(pos1, { pos2.x, pos1.y });
		DrawLine({ pos2.x, pos1.y }, pos2);
		DrawLine(pos2, { pos1.x, pos2.y });
		DrawLine({ pos1.x, pos2.y }, pos1);
	}

	void ConsoleGameEngine::DrawSprite(vec2d pos, Sprite* sprite)
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

	int ConsoleGameEngine::GetWidth()
	{
		return nScreenWidth;
	}

	int ConsoleGameEngine::GetHeight()
	{
		return nScreenHeight;
	}
}
