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

	using namespace def;

	class Example : public def::ConsoleGameEngine
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
#include <cstdint>

#pragma comment(lib, "winmm.lib")

#ifdef XBOX_CONTROLLER
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")
#endif

namespace def
{
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

	class ConsoleGameEngine
	{
	public:
		ConsoleGameEngine()
		{
			hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

			hWnd = GetConsoleWindow();

			hDC = GetDC(hWnd);

			sAppName = L"Undefined";
			sFont = L"Consolas";
		}

		virtual ~ConsoleGameEngine()
		{
			delete[] pScreen;
		}

	public:
		virtual bool OnUserCreate() = 0;
		virtual bool OnUserUpdate(float fDeltaTime) = 0;

		enum RCODE
		{
			RCODE_OK,
			RCODE_INVALID_SCREEN_SIZE,
			RCODE_INVALID_SCREEN_BUFFER,
			RCODE_INVALID_FONT,
			RCODE_INVALID_CONSOLE_MODE,
			RCODE_INVALID_SCREEN_INFO,
		};

		int ConstructConsole(int width = 120, int height = 40, int fontw = 4, int fonth = 4)
		{
			if (width <= 0 || height <= 0 || fontw <= 0 || fonth <= 0)
				return RCODE_INVALID_SCREEN_SIZE;

			nScreenWidth = width;
			nScreenHeight = height;

			nFontW = fontw;
			nFontH = fonth;

			hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

			if (hConsoleOut == INVALID_HANDLE_VALUE)
				return RCODE_INVALID_SCREEN_BUFFER;

			rRectWindow = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rRectWindow);

			COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
			if (!SetConsoleScreenBufferSize(hConsoleOut, coord))
				return RCODE_INVALID_SCREEN_SIZE;

			if (!SetConsoleActiveScreenBuffer(hConsoleOut))
				return RCODE_INVALID_SCREEN_BUFFER;

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = nFontW;
			cfi.dwFontSize.Y = nFontH;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, sFont.c_str());
			if (!SetCurrentConsoleFontEx(hConsoleOut, false, &cfi))
				return RCODE_INVALID_FONT;

			if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
				return RCODE_INVALID_CONSOLE_MODE;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
				return RCODE_INVALID_SCREEN_INFO;

			if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
				return RCODE_INVALID_SCREEN_SIZE;

			if (nScreenWidth > csbi.dwMaximumWindowSize.X)
				return RCODE_INVALID_SCREEN_SIZE;

			rRectWindow = { 0, 0, short(nScreenWidth - 1), short(nScreenHeight - 1) };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rRectWindow);

			pScreen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(pScreen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			return RCODE_OK;
		}

		void Run()
		{
			bGameThreadActive = true;

			std::thread t = std::thread(&def::ConsoleGameEngine::AppThread, this);
			t.join();
		}

	public:
		virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F);
		virtual void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);
		virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);
		virtual void DrawRectangle(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);
		virtual void FillRectangle(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);
		virtual void DrawCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);
		virtual void FillCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);
		virtual void DrawSprite(int x, int y, Sprite* sprite);
		virtual void DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);
		virtual void DrawWireFrameModel(std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short c = 0x2588, short col = 0x000F);
		virtual void DrawString(int x, int y, const std::wstring& text, short col = 0x000F);
		virtual void Clear(short c = 0x2588, short col = 0x000F);

		bool MakeSound(std::wstring sFilename, bool bLoop = false);
		bool Focused();

		int MouseX() const;
		int MouseY() const;

		KeyState GetMouse(short button) const;
		KeyState GetKey(short key) const;

		int ScreenWidth() const;
		int ScreenHeight() const;

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
					Keys[i] = { false, false, false };

				for (int i = 0; i < 5; i++)
					Mouse[i] = { false, false, false };

				while (bGameThreadActive)
				{
					tp2 = std::chrono::system_clock::now();
					std::chrono::duration<float> elapsedTime = tp2 - tp1;
					tp1 = tp2;
					fDeltaTime = elapsedTime.count();

					wchar_t buffer_title[256];
					swprintf_s(buffer_title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / fDeltaTime);
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
									bMouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
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
						nKeyNewState[i] = GetAsyncKeyState(i);

						Keys[i].bPressed = false;
						Keys[i].bReleased = false;

						if (nKeyNewState[i] != nKeyOldState[i])
						{
							if (nKeyNewState[i] & 0x8000)
							{
								Keys[i].bPressed = !Keys[i].bHeld;
								Keys[i].bHeld = true;
							}
							else
							{
								Keys[i].bReleased = true;
								Keys[i].bHeld = false;
							}
						}

						nKeyOldState[i] = nKeyNewState[i];
					}

					for (int m = 0; m < 5; m++)
					{
						Mouse[m].bPressed = false;
						Mouse[m].bReleased = false;

						if (bMouseNewState[m] != bMouseOldState[m])
						{
							if (bMouseNewState[m])
							{
								Mouse[m].bPressed = true;
								Mouse[m].bHeld = true;
							}
							else
							{
								Mouse[m].bReleased = true;
								Mouse[m].bHeld = false;
							}
						}

						bMouseOldState[m] = bMouseNewState[m];
					}

					WriteConsoleOutputW(hConsoleOut, pScreen, { (short)nScreenWidth, (short)nScreenHeight }, { 0,0 }, &rRectWindow);
				}
			}
		}

	protected:
		std::wstring sAppName;
		std::wstring sFont;

	private:
		CHAR_INFO* pScreen = nullptr;
		HANDLE hConsoleOut;
		HANDLE hConsoleIn;
		SMALL_RECT rRectWindow;
		HWND hWnd;
		HDC hDC;

		KeyState Keys[256];
		KeyState Mouse[5];

		short nKeyOldState[256];
		short nKeyNewState[256];

		bool bMouseOldState[5];
		bool bMouseNewState[5];

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

	bool ConsoleGameEngine::MakeSound(std::wstring sFilename, bool bLoop)
	{
		DWORD f = SND_ASYNC | SND_FILENAME;

		if (bLoop)
			f |= SND_LOOP;

		return (bool)PlaySoundW(sFilename.c_str(), nullptr, f);
	}

	bool ConsoleGameEngine::Focused()
	{
		return bFocused;
	}

	void ConsoleGameEngine::FillRectangle(int x, int y, int size_x, int size_y, short c, short col)
	{
		for (int i = x; i <= size_x; i++)
			for (int j = y; j <= size_y; j++)
				Draw(x, y, c, col);
	}

	void ConsoleGameEngine::DrawCircle(int x, int y, int radius, short c, short col)
	{
		if (radius <= 0) return;

		int x1 = 0;
		int y1 = radius;
		int p = 3 - 2 * radius;

		while (y1 >= x1)
		{
			Draw(x - x1, y - y1, c, col);	// upper left left
			Draw(x - y1, y - x1, c, col);	// upper upper left
			Draw(x + y1, y - x1, c, col);	// upper upper right
			Draw(x + x1, y - y1, c, col);	// upper right right
			Draw(x - x1, y + y1, c, col);	// lower left left
			Draw(x - y1, y + x1, c, col);	// lower lower left
			Draw(x + y1, y + x1, c, col);	// lower lower right
			Draw(x + x1, y + y1, c, col);	// lower right right

			if (p < 0) p += 4 * x1++ + 6;
			else p += 4 * (x1++ - y1--) + 10;
		}
	}

	void ConsoleGameEngine::FillCircle(int x, int y, int radius, short c, short col)
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

	void ConsoleGameEngine::Draw(int x, int y, short c, short col)
	{
		if (x >= 0 && x < nScreenWidth && y >= 0 && y < nScreenHeight)
		{
			pScreen[y * nScreenWidth + x].Char.UnicodeChar = c;
			pScreen[y * nScreenWidth + x].Attributes = col;
		}
	}

	void ConsoleGameEngine::DrawLine(int x1, int y1, int x2, int y2, short c, short col)
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
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						y = y + 1;
					else
						y = y - 1;

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
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
						x++;
					else
						x--;

					py = py + 2 * (dx1 - dy1);
				}

				Draw(x, y, c, col);
			}
		}
	}

	void ConsoleGameEngine::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		DrawLine(x1, y1, x2, y2, c, col);
		DrawLine(x2, y2, x3, y3, c, col);
		DrawLine(x3, y3, x1, y1, c, col);
	}

	void ConsoleGameEngine::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, c, col); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;

		bool changed1 = false;
		bool changed2 = false;

		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;

		// Sort vertices
		if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
		if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
		if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
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
		{   // swap values
			std::swap(dx1, dy1);
			changed1 = true;
		}
		if (dy2 > dx2)
		{   // swap values
			std::swap(dy2, dx2);
			changed2 = true;
		}

		e2 = (int)(dx2 >> 1);

		// Flat top, just process the second half
		if (y1 == y2)
			goto next;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;)
		{
			t1xp = 0; t2xp = 0;
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

			// process first line until y value is about to change
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
			// Move line
		next1:
			// process second line until y value is about to change
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

			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
			// Now increase y
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
		// Second half
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
		{   // swap values
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

			// process first line until y value is about to change
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
			// process second line until y value is about to change
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

	void ConsoleGameEngine::DrawRectangle(int x, int y, int size_x, int size_y, short c, short col)
	{
		for (int i = 0; i <= size_x; i++)
		{
			Draw(x + i, y, c, col);
			Draw(x + i, y + size_y, c, col);
		}

		for (int j = 0; j <= size_y; j++)
		{
			Draw(x, y + j, c, col);
			Draw(x + size_x, y + j, c, col);
		}
	}

	void ConsoleGameEngine::DrawSprite(int x, int y, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = 0; i < sprite->nWidth; i++)
		{
			for (int j = 0; j < sprite->nHeight; j++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + i, y + j, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void ConsoleGameEngine::DrawPartialSprite(int x, int y, int fx, int fy, int fsizex, int fsizey, Sprite* sprite)
	{
		if (sprite == nullptr) return;

		for (int i = fx, x1 = 0; i < fx + fsizex; i++, x1++)
		{
			for (int j = fy, y1 = 0; j < fy + fsizey; j++, y1++)
			{
				if (sprite->GetGlyph(i, j) != L' ')
					Draw(x + x1, y + y1, sprite->GetGlyph(i, j), sprite->GetColour(i, j));
			}
		}
	}

	void ConsoleGameEngine::DrawWireFrameModel(std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r, float s, short c, short col)
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
		for (int i = 0; i <= verts; i++)
		{
			int j = (i + 1);
			DrawLine((int)vecTransformedCoordinates[i % verts].first, (int)vecTransformedCoordinates[i % verts].second,
				(int)vecTransformedCoordinates[j % verts].first, (int)vecTransformedCoordinates[j % verts].second, c, col);
		}
	}

	void ConsoleGameEngine::DrawString(int x, int y, const std::wstring& text, short col)
	{
		for (size_t i = 0; i < text.size(); i++)
		{
			pScreen[y * nScreenWidth + x + i].Char.UnicodeChar = text[i];
			pScreen[y * nScreenWidth + x + i].Attributes = col;
		}
	}

	void ConsoleGameEngine::Clear(short c, short col)
	{
		FillRectangle(0, 0, nScreenWidth - 1, nScreenHeight - 1, c, col);
	}

	int ConsoleGameEngine::MouseX() const
	{
		return nMousePosX;
	}

	int ConsoleGameEngine::MouseY() const
	{
		return nMousePosY;
	}

	KeyState ConsoleGameEngine::GetMouse(short button) const
	{
		return Mouse[button];
	}

	KeyState ConsoleGameEngine::GetKey(short key) const
	{
		return Keys[key];
	}

	int ConsoleGameEngine::ScreenWidth() const
	{
		return nScreenWidth;
	}

	int ConsoleGameEngine::ScreenHeight() const
	{
		return nScreenHeight;
	}
}
