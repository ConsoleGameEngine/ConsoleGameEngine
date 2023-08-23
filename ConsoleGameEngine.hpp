#ifndef CONSOLE_GAME_ENGINE_HPP
#define CONSOLE_GAME_ENGINE_HPP

#pragma region consolegameengine_license
/***
*	BSD 3-Clause License

	Copyright (c) 2021, 2022, 2023 Alex
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
			for (int32_t i = 0; i < ScreenWidth(); i++)
				for (int32_t j = 0; j < ScreenHeight(); j++)
					Draw(i, j, PIXEL_SOLID, rand() % 15);

			return true;
		}
	};

	int32_t main()
	{
			Example demo;

			if (demo.ConstructConsole(120, 40, 12, 12) == rcode::OK)
				demo.Run();

			return 0;
	}
**/
#pragma endregion

#ifndef UNICODE
#error Enable Unicode in settings
#endif

#include <Windows.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <thread>
#include <string>
#include <cstdint>
#include <atomic>

#pragma comment(lib, "winmm.lib")

#undef min
#undef max

enum FG_COLORS : int16_t
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

enum BG_COLORS : int16_t
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

enum PIXEL_TYPE : int16_t
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
};

enum COMMON_LVB : uint16_t
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
	Sprite();
	Sprite(int32_t nWidth, int32_t nHeight);
	Sprite(std::wstring sFileName);

	~Sprite();

private:
	int16_t* m_Glyphs = nullptr;
	int16_t* m_Colours = nullptr;

public:
	int32_t nWidth = 0;
	int32_t nHeight = 0;

private:
	void Create(int32_t nWidth, int32_t nHeight);

public:
	void SetGlyph(int32_t x, int32_t y, wchar_t c);
	void SetColour(int32_t x, int32_t y, int16_t col);

	wchar_t GetGlyph(int32_t x, int32_t y);
	int16_t GetColour(int32_t x, int32_t y);

	bool Save(const std::wstring& sFileName);
	bool Load(const std::wstring& sFileName);
};

enum class rcode
{
	OK,
	INVALID_SCREEN_SIZE,
	INVALID_SCREEN_BUFFER,
	INVALID_FONT,
	INVALID_CONSOLE_MODE,
	INVALID_SCREEN_INFO,
};

class ConsoleGameEngine
{
public:
	ConsoleGameEngine();
	virtual ~ConsoleGameEngine();

public:
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fDeltaTime) = 0;

	rcode ConstructConsole(int32_t nWidth = 120, int32_t nHeight = 40, int32_t nFontWidth = 4, int32_t nFontHeight = 4);
	void Run();

public:
	bool MakeSound(const std::wstring& sFilename, bool bLoop);

	bool Focused();

	virtual void Draw(int32_t x, int32_t y, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawRectangle(int32_t x, int32_t y, int32_t sizeX, int32_t sizeY, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void FillRectangle(int32_t x, int32_t y, int32_t sizeX, int32_t sizeY, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawCircle(int32_t x, int32_t y, int32_t radius, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void FillCircle(int32_t x, int32_t y, int32_t radius, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawSprite(int32_t x, int32_t y, Sprite* sprite);
	virtual void DrawSpriteAlpha(int32_t x, int32_t y, Sprite* sprite);
	virtual void DrawPartialSprite(int32_t x, int32_t y, int32_t fx, int32_t fy, int32_t fsizeX, int32_t fsizeY, Sprite* sprite);
	virtual void DrawPartialSpriteAlpha(int32_t x, int32_t y, int32_t fx, int32_t fy, int32_t fsizeX, int32_t fsizeY, Sprite* sprite);
	virtual void DrawWireFrameModel(const std::vector<std::pair<float, float>>& modelCoordinates, float x, float y, float r, float s, wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);
	virtual void DrawString(int32_t x, int32_t y, const std::wstring& text, int16_t col = FG_WHITE);
	virtual void Clear(wchar_t c = PIXEL_SOLID, int16_t col = FG_WHITE);

	int32_t MouseX() const;
	int32_t MouseY() const;

	const KeyState& GetMouse(int16_t button) const;
	const KeyState& GetKey(int16_t key) const;

	int32_t ScreenWidth() const;
	int32_t ScreenHeight() const;

private:
	void AppThread();

protected:
	std::wstring sAppName;
	std::wstring sFont;

private:
	CHAR_INFO* m_pScreen = nullptr;
	HANDLE m_hConsoleOut;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rWindow;
	HWND m_hWnd;
	HDC m_hDC;

	KeyState m_Keys[256];
	KeyState m_Mouse[5];

	int16_t m_nKeyOldState[256];
	int16_t m_nKeyNewState[256];

	bool m_bMouseOldState[5]{ false };
	bool m_bMouseNewState[5]{ false };

	int32_t m_nMouseX;
	int32_t m_nMouseY;

	int32_t m_nScreenWidth;
	int32_t m_nScreenHeight;

	int32_t m_nFontWidth;
	int32_t m_nFontHeight;

	float m_fDeltaTime;

	std::thread m_thrGame;
	std::atomic<bool> m_bGameThreadActive;
	bool m_bFocused = true;
};

#ifdef CGE_IMPL
#undef CGE_IMPL

Sprite::Sprite()
{
	Create(8, 8);
}

Sprite::Sprite(int32_t nWidth, int32_t nHeight)
{
	Create(nWidth, nHeight);
}

Sprite::Sprite(std::wstring sFileName)
{
	if (!Load(sFileName))
		Create(8, 8);
}

Sprite::~Sprite()
{
	if (m_Glyphs != nullptr) delete[] m_Glyphs;
	if (m_Colours != nullptr) delete[] m_Colours;
}

void Sprite::Create(int32_t nWidth, int32_t nHeight)
{
	this->nWidth = nWidth;
	this->nHeight = nHeight;

	m_Glyphs = new int16_t[nWidth * nHeight];
	m_Colours = new int16_t[nWidth * nHeight];

	for (int32_t i = 0; i < nWidth * nHeight; i++)
	{
		m_Glyphs[i] = L' ';
		m_Colours[i] = FG_BLACK;
	}
}

void Sprite::SetGlyph(int32_t x, int32_t y, wchar_t c)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		m_Glyphs[y * nWidth + x] = c;
}

void Sprite::SetColour(int32_t x, int32_t y, int16_t c)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		m_Colours[y * nWidth + x] = c;
}

wchar_t Sprite::GetGlyph(int32_t x, int32_t y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_Glyphs[y * nWidth + x];

	return L' ';
}

int16_t Sprite::GetColour(int32_t x, int32_t y)
{
	if (x >= 0 && x < nWidth && y >= 0 && y < nHeight)
		return m_Colours[y * nWidth + x];

	return FG_BLACK;
}

bool Sprite::Save(const std::wstring& sFileName)
{
	FILE* f = nullptr;

	_wfopen_s(&f, sFileName.c_str(), L"wb");
	if (!f) return false;

	fwrite(&nWidth, sizeof(int), 1, f);
	fwrite(&nHeight, sizeof(int), 1, f);

	fwrite(m_Colours, sizeof(int16_t), nWidth * nHeight, f);
	fwrite(m_Glyphs, sizeof(int16_t), nWidth * nHeight, f);

	fclose(f);

	return true;
}

bool Sprite::Load(const std::wstring& sFileName)
{
	delete[] m_Glyphs;
	delete[] m_Colours;

	nWidth = 0;
	nHeight = 0;

	FILE* f = nullptr;

	_wfopen_s(&f, sFileName.c_str(), L"rb");
	if (!f) return false;

	fread(&nWidth, sizeof(int), 1, f);
	fread(&nHeight, sizeof(int), 1, f);

	Create(nWidth, nHeight);

	fread(m_Colours, sizeof(int16_t), nWidth * nHeight, f);
	fread(m_Glyphs, sizeof(int16_t), nWidth * nHeight, f);

	fclose(f);

	return true;
}

ConsoleGameEngine::ConsoleGameEngine()
{
	m_hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	m_hWnd = GetConsoleWindow();

	m_hDC = GetDC(m_hWnd);

	sAppName = L"Undefined";
	sFont = L"Consolas";
}

ConsoleGameEngine::~ConsoleGameEngine()
{
	delete[] m_pScreen;
}

rcode ConsoleGameEngine::ConstructConsole(int32_t nWidth, int32_t nHeight, int32_t nFontWidth, int32_t nFontHeight)
{
	if (nWidth <= 0 || nHeight <= 0 || nFontWidth <= 0 || nFontHeight <= 0)
		return rcode::INVALID_SCREEN_SIZE;

	m_nScreenWidth = nWidth;
	m_nScreenHeight = nHeight;

	m_nFontWidth = nFontWidth;
	m_nFontHeight = nFontHeight;

	m_hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	if (m_hConsoleOut == INVALID_HANDLE_VALUE)
		return rcode::INVALID_SCREEN_BUFFER;

	m_rWindow = { 0, 0, 1, 1 };
	SetConsoleWindowInfo(m_hConsoleOut, TRUE, &m_rWindow);

	COORD coord = { (int16_t)m_nScreenWidth, (int16_t)m_nScreenHeight };

	if (!SetConsoleScreenBufferSize(m_hConsoleOut, coord))  return rcode::INVALID_SCREEN_SIZE;
	if (!SetConsoleActiveScreenBuffer(m_hConsoleOut))		return rcode::INVALID_SCREEN_BUFFER;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = m_nFontWidth;
	cfi.dwFontSize.Y = m_nFontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy_s(cfi.FaceName, sFont.c_str());
	if (!SetCurrentConsoleFontEx(m_hConsoleOut, false, &cfi))
		return rcode::INVALID_FONT;

	if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return rcode::INVALID_CONSOLE_MODE;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(m_hConsoleOut, &csbi))
		return rcode::INVALID_SCREEN_INFO;

	if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
		return rcode::INVALID_SCREEN_SIZE;

	if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
		return rcode::INVALID_SCREEN_SIZE;

	m_rWindow = { 0, 0, int16_t(m_nScreenWidth - 1), int16_t(m_nScreenHeight - 1) };
	SetConsoleWindowInfo(m_hConsoleOut, TRUE, &m_rWindow);

	m_pScreen = new CHAR_INFO[m_nScreenWidth * m_nScreenHeight];
	memset(m_pScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

	return rcode::OK;
}

void ConsoleGameEngine::Run()
{
	m_bGameThreadActive = true;

	m_thrGame = std::thread(&ConsoleGameEngine::AppThread, this);
	if (m_thrGame.joinable()) m_thrGame.join();
}

bool ConsoleGameEngine::MakeSound(const std::wstring& sFilename, bool bLoop)
{
	DWORD f = SND_ASYNC | SND_FILENAME;
	if (bLoop) f |= SND_LOOP;

	return PlaySoundW(sFilename.c_str(), NULL, f);
}

bool ConsoleGameEngine::Focused()
{
	return m_bFocused;
}

void ConsoleGameEngine::Draw(int32_t x, int32_t y, wchar_t c, int16_t col)
{
	if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
	{
		m_pScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
		m_pScreen[y * m_nScreenWidth + x].Attributes = col;
	}
}

void ConsoleGameEngine::FillRectangle(int32_t x, int32_t y, int32_t sizeX, int32_t sizeY, wchar_t c, int16_t col)
{
	for (int32_t i = 0; i < sizeX; i++)
		for (int32_t j = 0; j < sizeY; j++)
			Draw(x + i, y + j, c, col);
}

void ConsoleGameEngine::DrawCircle(int32_t x, int32_t y, int32_t radius, wchar_t c, int16_t col)
{
	if (radius <= 0) return;

	int32_t x1 = 0;
	int32_t y1 = radius;
	int32_t p = 3 - 2 * radius;

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

void ConsoleGameEngine::FillCircle(int32_t x, int32_t y, int32_t radius, wchar_t c, int16_t col)
{
	if (radius <= 0) return;

	int32_t x1 = 0;
	int32_t y1 = radius;
	int32_t p = 3 - 2 * radius;

	auto drawline = [&](int32_t sx, int32_t ex, int32_t ny)
	{
		for (int32_t i = sx; i <= ex; i++)
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

void ConsoleGameEngine::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, wchar_t c, int16_t col)
{
	int32_t x, y, xe, ye;

	int32_t dx = x2 - x1;
	int32_t dy = y2 - y1;

	int32_t dx1 = abs(dx);
	int32_t dy1 = abs(dy);

	int32_t px = 2 * dy1 - dx1;
	int32_t py = 2 * dx1 - dy1;

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

		for (int32_t i = 0; x < xe; i++)
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

		for (int32_t i = 0; y < ye; i++)
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

void ConsoleGameEngine::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, wchar_t c, int16_t col)
{
	DrawLine(x1, y1, x2, y2, c, col);
	DrawLine(x2, y2, x3, y3, c, col);
	DrawLine(x3, y3, x1, y1, c, col);
}

void ConsoleGameEngine::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, wchar_t c, int16_t col)
{
	auto drawline = [&](int32_t sx, int32_t ex, int32_t ny) { for (int32_t i = sx; i <= ex; i++) Draw(i, ny, c, col); };

	int32_t t1x, t2x, y, minx, maxx, t1xp, t2xp;

	bool changed1 = false;
	bool changed2 = false;

	int32_t signx1, signx2, dx1, dy1, dx2, dy2;
	int32_t e1, e2;

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

	for (int32_t i = 0; i < dx1;)
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

	for (int32_t i = 0; i <= dx1; i++)
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

void ConsoleGameEngine::DrawRectangle(int32_t x, int32_t y, int32_t sizeX, int32_t sizeY, wchar_t c, int16_t col)
{
	for (int32_t i = 0; i <= sizeX; i++)
	{
		Draw(x + i, y, c, col);
		Draw(x + i, y + sizeY, c, col);
	}

	for (int32_t j = 0; j <= sizeY; j++)
	{
		Draw(x, y + j, c, col);
		Draw(x + sizeX, y + j, c, col);
	}
}

void ConsoleGameEngine::DrawSprite(int32_t x, int32_t y, Sprite* sprite)
{
	if (sprite == nullptr) return;

	for (int32_t i = 0; i < sprite->nWidth; i++)
		for (int32_t j = 0; j < sprite->nHeight; j++)
			Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
}

void ConsoleGameEngine::DrawSpriteAlpha(int32_t x, int32_t y, Sprite* sprite)
{
	if (sprite == nullptr) return;

	for (int32_t i = 0; i < sprite->nWidth; i++)
		for (int32_t j = 0; j < sprite->nHeight; j++)
			if (sprite->GetGlyph(i, j) != L' ')
				Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
}

void ConsoleGameEngine::DrawPartialSprite(int32_t x, int32_t y, int32_t fx, int32_t fy, int32_t fsizeX, int32_t fsizeY, Sprite* sprite)
{
	if (sprite == nullptr) return;

	for (int32_t i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
		for (int32_t j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
			Draw(x + x1, y + y1, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
}

void ConsoleGameEngine::DrawPartialSpriteAlpha(int32_t x, int32_t y, int32_t fx, int32_t fy, int32_t fsizeX, int32_t fsizeY, Sprite* sprite)
{
	if (sprite == nullptr) return;

	for (int32_t i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
		for (int32_t j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
		{
			if (sprite->GetGlyph(i, j) != L' ')
				Draw(x + x1, y + y1, sprite->GetGlyph(i, j), sprite->GetColour(i, j) | sprite->GetColour(i, j) * 16);
		}
}

void ConsoleGameEngine::DrawWireFrameModel(const std::vector<std::pair<float, float>>& modelCoordinates, float x, float y, float r, float s, wchar_t c, int16_t col)
{
	size_t nVerts = modelCoordinates.size();
	std::vector<std::pair<float, float>> vecTransformedCoordinates(nVerts);

	for (size_t i = 0; i < nVerts; i++)
	{
		vecTransformedCoordinates[i].first = (modelCoordinates[i].first * cos(r) - modelCoordinates[i].second * sin(r)) * s + x;
		vecTransformedCoordinates[i].second = (modelCoordinates[i].first * sin(r) + modelCoordinates[i].second * cos(r)) * s + y;
	}

	for (size_t i = 0; i <= nVerts; i++)
	{
		size_t j = (i + 1);
		DrawLine((int)vecTransformedCoordinates[i % nVerts].first, (int)vecTransformedCoordinates[i % nVerts].second,
			(int)vecTransformedCoordinates[j % nVerts].first, (int)vecTransformedCoordinates[j % nVerts].second, c, col);
	}
}

void ConsoleGameEngine::DrawString(int32_t x, int32_t y, const std::wstring& text, int16_t col)
{
	for (size_t i = 0; i < text.size(); i++)
	{
		m_pScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = text[i];
		m_pScreen[y * m_nScreenWidth + x + i].Attributes = col;
	}
}

void ConsoleGameEngine::Clear(wchar_t c, int16_t col)
{
	FillRectangle(0, 0, m_nScreenWidth, m_nScreenHeight, c, col);
}

int32_t ConsoleGameEngine::MouseX() const
{
	return m_nMouseX;
}

int32_t ConsoleGameEngine::MouseY() const
{
	return m_nMouseY;
}

const KeyState& ConsoleGameEngine::GetMouse(int16_t button) const
{
	return m_Mouse[button];
}

const KeyState& ConsoleGameEngine::GetKey(int16_t key) const
{
	return m_Keys[key];
}

int32_t ConsoleGameEngine::ScreenWidth() const
{
	return m_nScreenWidth;
}

int32_t ConsoleGameEngine::ScreenHeight() const
{
	return m_nScreenHeight;
}

void ConsoleGameEngine::AppThread()
{
	if (!OnUserCreate())
		m_bGameThreadActive = false;

	if (m_bGameThreadActive)
	{
		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		for (int32_t i = 0; i < 256; i++)
			m_Keys[i] = { false, false, false };

		for (int32_t i = 0; i < 5; i++)
			m_Mouse[i] = { false, false, false };

		while (m_bGameThreadActive)
		{
			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;
			m_fDeltaTime = elapsedTime.count();

			wchar_t title[256];
			swprintf_s(title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / m_fDeltaTime);
			SetConsoleTitleW(title);

			if (!OnUserUpdate(m_fDeltaTime))
				m_bGameThreadActive = false;

			INPUT_RECORD inBuf[32];
			DWORD nEvents = 0;
			GetNumberOfConsoleInputEvents(m_hConsoleIn, &nEvents);
			if (nEvents > 0)
				ReadConsoleInputW(m_hConsoleIn, inBuf, nEvents, &nEvents);

			for (DWORD i = 0; i < nEvents; i++)
			{
				switch (inBuf[i].EventType)
				{
				case FOCUS_EVENT:
				m_bFocused = inBuf[i].Event.FocusEvent.bSetFocus;
				break;

				case WINDOW_BUFFER_SIZE_EVENT:
				{
					m_nScreenWidth = (int32_t)inBuf[i].Event.WindowBufferSizeEvent.dwSize.X;
					m_nScreenHeight = (int32_t)inBuf[i].Event.WindowBufferSizeEvent.dwSize.Y;
				}
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
						for (int32_t m = 0; m < 5; m++)
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

			for (int32_t i = 0; i < 256; i++)
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

			for (int32_t i = 0; i < 5; i++)
			{
				m_Mouse[i].bPressed = false;
				m_Mouse[i].bReleased = false;

				if (m_bMouseNewState[i] != m_bMouseOldState[i])
				{
					if (m_bMouseNewState[i])
					{
						m_Mouse[i].bPressed = true;
						m_Mouse[i].bHeld = true;
					}
					else
					{
						m_Mouse[i].bReleased = true;
						m_Mouse[i].bHeld = false;
					}
				}

				m_bMouseOldState[i] = m_bMouseNewState[i];
			}

			WriteConsoleOutputW(m_hConsoleOut, m_pScreen, { (int16_t)m_nScreenWidth, (int16_t)m_nScreenHeight }, { 0, 0 }, &m_rWindow);
		}
	}
}

#endif

#endif
