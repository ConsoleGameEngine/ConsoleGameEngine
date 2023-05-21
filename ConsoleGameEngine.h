#pragma once

#pragma region consolegameengine_license
/***
*	BSD 3-Clause License

	Copyright (c) 2021, 2022 Alex
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

#pragma region consolegameengine_sample
/**
* Example (engine only supports .spr files, check [this](https://github.com/defini7/SpriteEditor) for editing .spr files):
	#include "ConsoleGameEngine.h"

	class Example : public ConsoleGameEngine
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
			for (int i = 0; i < ScreenWidth(); i++)
				for (int j = 0; j < ScreenHeight(); j++)
					Draw(i, j, PIXEL_SOLID, rand() % 15);

			return true;
		}
	};

	int main()
	{
			Example demo;

			if (demo.ConstructConsole(120, 40, 12, 12) == RCODE_OK)
				demo.Run();

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
#include <cstdint>

#pragma comment(lib, "winmm.lib")

#ifdef XBOX_CONTROLLER
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")
#endif

enum FG_COLORS : short
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

enum BG_COLORS : short
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

enum PIXEL_TYPE : short
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
};

enum COMMON_LVB : unsigned short
{
	CL_GRID_HORIZONTAL = 0x400,
	CL_GRID_LVERTICAL = 0x0800,
	CL_GRID_RVERTICAL = 0x1000,
	CL_UNDERSCORE = 0x8000
};

struct KeyState
{
	bool bHeld;
	bool bReleased;
	bool bPressed;
};

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

	~Sprite()
	{
		if (m_Glyphs != nullptr) delete[] m_Glyphs;
		if (m_Colours != nullptr) delete[] m_Colours;
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
	void SetGlyph(int x, int y, short c)
	{
		if (x > 0 || x < nWidth || y > 0 || y < nHeight)
			m_Glyphs[y * nWidth + x] = c;
	}

	void SetColour(int x, int y, short c)
	{
		if (x > 0 || x < nWidth || y > 0 || y < nHeight)
			m_Colours[y * nWidth + x] = c;
	}

	short GetGlyph(int x, int y)
	{
		if (x > 0 || x < nWidth || y > 0 || y < nHeight)
			return m_Glyphs[y * nWidth + x];

		return L' ';
	}

	short GetColour(int x, int y)
	{
		if (x > 0 || x < nWidth || y > 0 || y < nHeight)
			return m_Colours[y * nWidth + x];

		return FG_BLACK;
	}

	bool Save(const std::wstring& sFile)
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

enum RCODE
{
	RCODE_OK,
	RCODE_INVALID_SCREEN_SIZE,
	RCODE_INVALID_SCREEN_BUFFER,
	RCODE_INVALID_FONT,
	RCODE_INVALID_CONSOLE_MODE,
	RCODE_INVALID_SCREEN_INFO,
};

class ConsoleGameEngine
{
public:
	ConsoleGameEngine()
	{
		m_hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		m_hWnd = GetConsoleWindow();

		m_hDC = GetDC(m_hWnd);

		sAppName = L"Undefined";
		sFont = L"Consolas";
	}

	virtual ~ConsoleGameEngine()
	{
		delete[] m_pScreen;
	}

public:
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fDeltaTime) = 0;

	int ConstructConsole(int width = 120, int height = 40, int fontW = 4, int fontH = 4)
	{
		if (width <= 0 || height <= 0 || fontW <= 0 || fontH <= 0)
			return RCODE_INVALID_SCREEN_SIZE;

		m_nScreenWidth = width;
		m_nScreenHeight = height;

		m_nFontW = fontW;
		m_nFontH = fontH;

		m_hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

		if (m_hConsoleOut == INVALID_HANDLE_VALUE)
			return RCODE_INVALID_SCREEN_BUFFER;

		m_rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(m_hConsoleOut, TRUE, &m_rectWindow);

		COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };

		if (!SetConsoleScreenBufferSize(m_hConsoleOut, coord))  return RCODE_INVALID_SCREEN_SIZE;
		if (!SetConsoleActiveScreenBuffer(m_hConsoleOut))		return RCODE_INVALID_SCREEN_BUFFER;

		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = m_nFontW;
		cfi.dwFontSize.Y = m_nFontH;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		wcscpy_s(cfi.FaceName, sFont.c_str());
		if (!SetCurrentConsoleFontEx(m_hConsoleOut, false, &cfi))
			return RCODE_INVALID_FONT;

		if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			return RCODE_INVALID_CONSOLE_MODE;

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(m_hConsoleOut, &csbi))
			return RCODE_INVALID_SCREEN_INFO;

		if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
			return RCODE_INVALID_SCREEN_SIZE;

		if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
			return RCODE_INVALID_SCREEN_SIZE;

		m_rectWindow = { 0, 0, (short)(m_nScreenWidth - 1), (short)(m_nScreenHeight - 1) };
		SetConsoleWindowInfo(m_hConsoleOut, TRUE, &m_rectWindow);

		m_pScreen = new CHAR_INFO[m_nScreenWidth * m_nScreenHeight];
		memset(m_pScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

		return RCODE_OK;
	}

	void Run()
	{
		m_bGameThreadActive = true;

		std::thread t = std::thread(&ConsoleGameEngine::AppThread, this);
		t.join();
	}

public:
	bool MakeSound(std::wstring sFilename, bool bLoop)
	{
		DWORD f = SND_ASYNC | SND_FILENAME;

		if (bLoop)
			f |= SND_LOOP;

		return (bool)PlaySoundW(sFilename.c_str(), nullptr, f);
	}

	bool Focused()
	{
		return m_bFocused;
	}

	virtual void Draw(int x, int y, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			m_pScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
			m_pScreen[y * m_nScreenWidth + x].Attributes = col;
		}
	}

	virtual void FillRectangle(int x, int y, int sizeX, int sizeY, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		for (int i = 0; i < sizeX; i++)
			for (int j = 0; j < sizeY; j++)
				Draw(x + i, y + j, c, col);
	}

	virtual void DrawCircle(int x, int y, int radius, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		if (radius <= 0) return;

		int x1 = 0;
		int y1 = radius;
		int p = 3 - 2 * radius;

		while (y1 >= x1)
		{
			Draw(x - x1, y - y1, c, col);
			Draw(x - y1, y - x1, c, col);
			Draw(x + y1, y - x1, c, col);
			Draw(x + x1, y - y1, c, col);
			Draw(x - x1, y + y1, c, col);
			Draw(x - y1, y + x1, c, col);
			Draw(x + y1, y + x1, c, col);
			Draw(x + x1, y + y1, c, col);

			if (p < 0) p += 4 * x1++ + 6;
			else p += 4 * (x1++ - y1--) + 10;
		}
	}

	virtual void FillCircle(int x, int y, int radius, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		if (radius <= 0) return;

		int x1 = 0;
		int y1 = radius;
		int p = 3 - 2 * radius;

		auto drawline = [&](int sx, int ex, int ny)
		{
			for (int i = sx; i <= ex; i++)
				Draw(i, ny, c, col);
		};

		while (y1 >= x1)
		{
			drawline(x - x1, x + x1, y - y1);
			drawline(x - y1, x + y1, y - x1);
			drawline(x - x1, x + x1, y + y1);
			drawline(x - y1, x + y1, y + x1);

			if (p < 0) p += 4 * x1++ + 6;
			else p += 4 * (x1++ - y1--) + 10;
		}
	}

	virtual void DrawLine(int x1, int y1, int x2, int y2, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		int x, y, xe, ye;

		int dx = x2 - x1;
		int dy = y2 - y1;

		int dx1 = abs(dx);
		int dy1 = abs(dy);

		int px = 2 * dy1 - dx1;
		int py = 2 * dx1 - dy1;

		if (dy1 <= dx1)
		{
			if (dx >= 0)
			{
				x = x1;
				y = y1;
				xe = x2;
			}
			else
			{
				x = x2;
				y = y2;
				xe = x1;
			}

			Draw(x, y, c, col);

			for (int i = 0; x < xe; i++)
			{
				x++;

				if (px < 0)
					px = px + 2 * dy1;
				else
				{
					y += ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) ? 1 : -1;
					px = px + 2 * (dy1 - dx1);
				}

				Draw(x, y, c, col);
			}
		}
		else
		{
			if (dy >= 0)
			{
				x = x1;
				y = y1;
				ye = y2;
			}
			else
			{
				x = x2;
				y = y2;
				ye = y1;
			}

			Draw(x, y, c, col);

			for (int i = 0; y < ye; i++)
			{
				y++;

				if (py <= 0)
					py = py + 2 * dx1;
				else
				{
					x += ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) ? 1 : -1;
					py = py + 2 * (dx1 - dy1);
				}

				Draw(x, y, c, col);
			}
		}
	}

	virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		DrawLine(x1, y1, x2, y2, c, col);
		DrawLine(x2, y2, x3, y3, c, col);
		DrawLine(x3, y3, x1, y1, c, col);
	}

	virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;

		bool changed1 = false;
		bool changed2 = false;

		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;

		if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
		if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
		if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

		t1x = t2x = x1; y = y1;
		dx1 = x2 - x1;

		if (dx1 < 0)
		{
			dx1 = -dx1;
			signx1 = -1;
		}
		else
			signx1 = 1;

		dy1 = y2 - y1;
		dx2 = x3 - x1;

		if (dx2 < 0)
		{
			dx2 = -dx2;
			signx2 = -1;
		}
		else
			signx2 = 1;

		dy2 = y3 - y1;

		if (dy1 > dx1)
		{
			std::swap(dx1, dy1);
			changed1 = true;
		}

		if (dy2 > dx2)
		{
			std::swap(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);

		if (y1 == y2) goto next;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;)
		{
			t1xp = 0;
			t2xp = 0;

			if (t1x < t2x)
			{
				minx = t1x;
				maxx = t2x;
			}
			else
			{
				minx = t2x;
				maxx = t1x;
			}

			while (i < dx1)
			{
				i++;
				e1 += dy1;

				while (e1 >= dx1)
				{
					e1 -= dx1;

					if (changed1)
						t1xp = signx1;
					else
						goto next1;
				}

				if (changed1)
					break;
				else
					t1x += signx1;
			}

		next1:
			while (1)
			{
				e2 += dy2;
				while (e2 >= dx2)
				{
					e2 -= dx2;
					if (changed2)
						t2xp = signx2;
					else
						goto next2;
				}
				if (changed2)
					break;
				else
					t2x += signx2;
			}

		next2:
			if (minx > t1x)
				minx = t1x;

			if (minx > t2x)
				minx = t2x;

			if (maxx < t1x)
				maxx = t1x;

			if (maxx < t2x)
				maxx = t2x;

			drawline(minx, maxx, y);

			if (!changed1)
				t1x += signx1;

			t1x += t1xp;

			if (!changed2)
				t2x += signx2;

			t2x += t2xp;
			y += 1;

			if (y == y2)
				break;

		}

	next:
		dx1 = x3 - x2;

		if (dx1 < 0)
		{
			dx1 = -dx1;
			signx1 = -1;
		}
		else
			signx1 = 1;

		dy1 = y3 - y2;
		t1x = x2;

		if (dy1 > dx1)
		{
			std::swap(dy1, dx1);
			changed1 = true;
		}
		else
			changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++)
		{
			t1xp = 0;
			t2xp = 0;

			if (t1x < t2x)
			{
				minx = t1x;
				maxx = t2x;
			}
			else
			{
				minx = t2x;
				maxx = t1x;
			}

			while (i < dx1)
			{
				e1 += dy1;

				while (e1 >= dx1)
				{
					e1 -= dx1;
					if (changed1)
					{
						t1xp = signx1;
						break;
					}
					else
						goto next3;
				}

				if (changed1)
					break;
				else
					t1x += signx1;

				if (i < dx1)
					i++;
			}

		next3:
			while (t2x != x3)
			{
				e2 += dy2;

				while (e2 >= dx2)
				{
					e2 -= dx2;

					if (changed2)
						t2xp = signx2;
					else
						goto next4;
				}

				if (changed2)
					break;
				else
					t2x += signx2;
			}

		next4:
			if (minx > t1x)
				minx = t1x;

			if (minx > t2x)
				minx = t2x;

			if (maxx < t1x)
				maxx = t1x;

			if (maxx < t2x)
				maxx = t2x;

			drawline(minx, maxx, y);

			if (!changed1)
				t1x += signx1;

			t1x += t1xp;

			if (!changed2)
				t2x += signx2;

			t2x += t2xp;
			y += 1;

			if (y > y3)
				return;
		}
	}

	virtual void DrawRectangle(int x, int y, int sizeX, int sizeY, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		for (int i = 0; i <= sizeX; i++)
		{
			Draw(x + i, y, c, col);
			Draw(x + i, y + sizeY, c, col);
		}

		for (int j = 0; j <= sizeY; j++)
		{
			Draw(x, y + j, c, col);
			Draw(x + sizeX, y + j, c, col);
		}
	}

	virtual void DrawSprite(int x, int y, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = 0; i < sprite->nWidth; i++)
			for (int j = 0; j < sprite->nHeight; j++)
				Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
	}

	void DrawSpriteAlpha(int x, int y, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = 0; i < sprite->nWidth; i++)
			for (int j = 0; j < sprite->nHeight; j++)
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
	}
	
	virtual void DrawPartialSprite(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
			for (int j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
				Draw(x + x1, y + y1, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
	}

	virtual void DrawPartialSpriteAlpha(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
			for (int j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + x1, y + y1, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
	}

	virtual void DrawWireFrameModel(const std::vector<std::pair<float, float>>& modelCoordinates, float x, float y, float r, float s, short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int nVerts = modelCoordinates.size();
		vecTransformedCoordinates.resize(nVerts);

		// Rotate
		for (int i = 0; i < nVerts; i++)
		{
			vecTransformedCoordinates[i].first = modelCoordinates[i].first * cos(r) - modelCoordinates[i].second * sin(r);
			vecTransformedCoordinates[i].second = modelCoordinates[i].first * sin(r) + modelCoordinates[i].second * cos(r);
		}

		// Scale
		for (int i = 0; i < nVerts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < nVerts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		for (int i = 0; i <= nVerts; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % nVerts].first, (int)vecTransformedCoordinates[i % nVerts].second,
				(int)vecTransformedCoordinates[j % nVerts].first, (int)vecTransformedCoordinates[j % nVerts].second, c, col);
		}
	}

	virtual void DrawString(int x, int y, const std::wstring& text, short col = FG_WHITE)
	{
		for (size_t i = 0; i < text.size(); i++)
		{
			m_pScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = text[i];
			m_pScreen[y * m_nScreenWidth + x + i].Attributes = col;
		}
	}

	virtual void Clear(short c = PIXEL_SOLID, short col = FG_WHITE)
	{
		FillRectangle(0, 0, m_nScreenWidth, m_nScreenHeight, c, col);
	}

	int MouseX() const
	{
		return m_nMouseX;
	}

	int MouseY() const
	{
		return m_nMouseY;
	}

	const KeyState& GetMouse(short button) const
	{
		return m_Mouse[button];
	}

	const KeyState& GetKey(short key) const
	{
		return m_Keys[key];
	}

	int ScreenWidth() const
	{
		return m_nScreenWidth;
	}

	int ScreenHeight() const
	{
		return m_nScreenHeight;
	}

private:
	void AppThread()
	{
		if (!OnUserCreate())
			m_bGameThreadActive = false;

		if (m_bGameThreadActive)
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			for (int i = 0; i < 256; i++)
				m_Keys[i] = { false, false, false };

			for (int i = 0; i < 5; i++)
				m_Mouse[i] = { false, false, false };

			while (m_bGameThreadActive)
			{
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				m_fDeltaTime = elapsedTime.count();

				wchar_t buffer_title[256];
				swprintf_s(buffer_title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / m_fDeltaTime);
				SetConsoleTitleW(buffer_title);

				if (!OnUserUpdate(m_fDeltaTime))
					m_bGameThreadActive = false;

				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
				if (events > 0)
					ReadConsoleInputW(m_hConsoleIn, inBuf, events, &events);

				for (DWORD i = 0; i < events; i++)
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
						m_bFocused = inBuf[i].Event.FocusEvent.bSetFocus;
						break;

					case MOUSE_EVENT:
					{
						switch (inBuf[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							m_nMouseX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							m_nMouseY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						}
						break;

						case 0:
						{
							for (int m = 0; m < 5; m++)
								m_bMouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
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
					m_nKeyNewState[i] = GetAsyncKeyState(i);

					m_Keys[i].bPressed = false;
					m_Keys[i].bReleased = false;

					if (m_nKeyNewState[i] != m_nKeyOldState[i])
					{
						if (m_nKeyNewState[i] & 0x8000)
						{
							m_Keys[i].bPressed = !m_Keys[i].bHeld;
							m_Keys[i].bHeld = true;
						}
						else
						{
							m_Keys[i].bReleased = true;
							m_Keys[i].bHeld = false;
						}
					}

					m_nKeyOldState[i] = m_nKeyNewState[i];
				}

				for (int m = 0; m < 5; m++)
				{
					m_Mouse[m].bPressed = false;
					m_Mouse[m].bReleased = false;

					if (m_bMouseNewState[m] != m_bMouseOldState[m])
					{
						if (m_bMouseNewState[m])
						{
							m_Mouse[m].bPressed = true;
							m_Mouse[m].bHeld = true;
						}
						else
						{
							m_Mouse[m].bReleased = true;
							m_Mouse[m].bHeld = false;
						}
					}

					m_bMouseOldState[m] = m_bMouseNewState[m];
				}

				WriteConsoleOutputW(m_hConsoleOut, m_pScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
			}
		}
	}

protected:
	std::wstring sAppName;
	std::wstring sFont;

private:
	CHAR_INFO* m_pScreen = nullptr;
	HANDLE m_hConsoleOut;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	HWND m_hWnd;
	HDC m_hDC;

	KeyState m_Keys[256];
	KeyState m_Mouse[5];

	short m_nKeyOldState[256];
	short m_nKeyNewState[256];

	bool m_bMouseOldState[5]{false};
	bool m_bMouseNewState[5]{false};

	int m_nMouseX;
	int m_nMouseY;

	int m_nScreenWidth;
	int m_nScreenHeight;

	int m_nFontW;
	int m_nFontH;

	float m_fDeltaTime;

	std::atomic<bool> m_bGameThreadActive;
	bool m_bFocused = true;
};
