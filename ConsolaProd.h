#ifdef DEF_CP_APP

#pragma once
#pragma comment(lib, "winmm.lib")

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

#pragma region EXAMPLE
/**
* Example (engine only supports .spr files, check [this](https://github.com/defini7/lab/tree/main/Sprite_Editor) for editing .spr files):
	#include "ConsolaProd.hpp"

	class Example : public def::ConsolaProd
	{
	public:
		Example()
		{
			sAppName = L"Example";
		}

	protected:
		virtual bool OnUserCreate() override
		{
			return true;
		}

		virtual bool OnUserUpdate(float dt) override
		{
			return true;
		}
	};

	int main()
	{
		Example demo;
		if (demo.Construct(256, 240, 4, 4))
			demo.Start();
		else
			std::wcerr << "Could not construct console" << std::endl;

		return 0;
	}
**/
#pragma endregion

#ifndef UNICODE
#error Enable Unicode in settings
#endif

#define _CRT_SECURE_NOWARNINGS
#define _SILENCE_CXX17_STRSTREAM_DEPRECATION_WARNING
#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_CODECVT_DEPRECATION_WARNINGS

#include <iostream>
#include <Windows.h>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>
#include <strstream>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <thread>
#include <atomic>
#include <string>

namespace def
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

	namespace Pixel
	{
		int SOLID = 0x2588,
			THREEQUARTERS = 0x2593,
			HALF = 0x2592,
			QUARTER = 0x2591;
	}

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

	struct vi2d
	{
		int x;
		int y;
	};

	struct vf2d
	{
		float x;
		float y;
	};

	struct KeyState
	{
		bool bHeld;
		bool bReleased;
		bool bPressed;
	};

	const double PI = 2.0f * acos(0.0f);

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
				m_Colours[i] = FG::BLACK;
			}
		}

	public:
		void SetGlyph(vi2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Glyphs[pos.y * nWidth + pos.x] = c;
		}

		void SetColour(vi2d pos, short c)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return;
			else
				m_Colours[pos.y * nWidth + pos.x] = c;
		}

		short GetGlyph(vi2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return L' ';
			else
				return m_Glyphs[pos.y * nWidth + pos.x];
		}

		short GetColour(vi2d pos)
		{
			if (pos.x < 0 || pos.x >= nWidth || pos.y < 0 || pos.y >= nHeight)
				return FG::BLACK;
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

		bool Run(int width = 120, int height = 40, int fontw = 4, int fonth = 4)
		{
			if (width <= 0 || height <= 0 || fontw <= 0 || fonth <= 0)
				return false;

			nScreenWidth = width;
			nScreenHeight = height;

			nFontW = fontw;
			nFontH = fonth;

			hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

			if (hConsoleOut == INVALID_HANDLE_VALUE)
				return false;

			rectWindow = { 0, 0, 1, 1 };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);

			COORD coord = { (short)nScreenWidth, (short)nScreenHeight };
			if (!SetConsoleScreenBufferSize(hConsoleOut, coord))
				return false;

			if (!SetConsoleActiveScreenBuffer(hConsoleOut))
				return false;

			CONSOLE_FONT_INFOEX cfi;
			cfi.cbSize = sizeof(cfi);
			cfi.nFont = 0;
			cfi.dwFontSize.X = nFontW;
			cfi.dwFontSize.Y = nFontH;
			cfi.FontFamily = FF_DONTCARE;
			cfi.FontWeight = FW_NORMAL;

			wcscpy_s(cfi.FaceName, sFont.c_str());
			if (!SetCurrentConsoleFontEx(hConsoleOut, false, &cfi))
				return false;

			if (!SetConsoleMode(hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
				return false;

			CONSOLE_SCREEN_BUFFER_INFO csbi;
			if (!GetConsoleScreenBufferInfo(hConsoleOut, &csbi))
				return false;
			if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
				return false;
			if (nScreenWidth > csbi.dwMaximumWindowSize.X)
				return false;

			rectWindow = { 0, 0, short(nScreenWidth - 1), short(nScreenHeight - 1) };
			SetConsoleWindowInfo(hConsoleOut, TRUE, &rectWindow);
			screen = new CHAR_INFO[nScreenWidth * nScreenHeight];
			memset(screen, 0, sizeof(CHAR_INFO) * nScreenWidth * nScreenHeight);

			bGameThreadActive = true;
			std::thread t = std::thread(&def::ConsolaProd::AppThread, this);
			t.join();

			return true;
		}

		void Stop(std::wstring reason = L"GAME WAS STOPED", int code = 0)
		{
			bGameThreadActive = false;
			std::wcout << reason << std::endl;
			exit(code);
		}

	public:
		void Draw(vi2d pos, short c = 0x2588, short col = 0x000F);
		void Draw(int x, int y, short c = 0x2588, short col = 0x000F);

		void DrawLine(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawLine(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);

		void DrawTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		void FillTriangle(vi2d pos1, vi2d pos2, vi2d pos3, short c = 0x2588, short col = 0x000F);
		void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, short c = 0x2588, short col = 0x000F);

		void DrawRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void DrawRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		void DrawRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		void DrawRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		void FillRectangle(vi2d pos1, vi2d pos2, short c = 0x2588, short col = 0x000F);
		void FillRectangle(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F);
		void FillRectangleS(vi2d pos, vi2d size, short c = 0x2588, short col = 0x000F);
		void FillRectangleS(int x, int y, int size_x, int size_y, short c = 0x2588, short col = 0x000F);

		void DrawCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		void DrawCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		void FillCircle(vi2d pos, int radius, short c = 0x2588, short col = 0x000F);
		void FillCircle(int x, int y, int radius, short c = 0x2588, short col = 0x000F);

		void DrawSprite(vi2d pos, Sprite* sprite);
		void DrawSprite(int x, int y, Sprite* sprite);

		void DrawPartialSprite(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite);
		void DrawPartialSprite(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);

		void DrawPartialSpriteS(vi2d pos, vi2d fpos1, vi2d fpos2, Sprite* sprite);
		void DrawPartialSpriteS(int x, int y, int fx1, int fy1, int fx2, int fy2, Sprite* sprite);

		void DrawWireFrameModel(std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, short c = Pixel::SOLID, short col = FG::WHITE);

		void DrawString(vi2d pos, std::wstring text, short c = 0x2588, short col = 0x000F);
		void DrawString(int x, int y, std::wstring text, short c = 0x2588, short col = 0x000F);

		void Clear(short c, short col = 0x000F);
		bool Focused();

		vi2d GetMouse();
		int GetMouseX();
		int GetMouseY();
		vf2d GetMouseF();

		KeyState GetMouse(short button);
		KeyState GetKey(short key);

		int GetScreenWidth();
		int GetScreenHeight();
		vi2d GetScreenSize();
		vf2d GetScreenSizeF();

		std::vector<int> AnyKeyHeld();
		std::vector<int> AnyKeyPressed();
		std::vector<int> AnyKeyReleased();

	private:
		void AppThread()
		{
			auto tp1 = std::chrono::system_clock::now();
			auto tp2 = std::chrono::system_clock::now();

			tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsedTime = tp2 - tp1;
			tp1 = tp2;

			if (!OnUserCreate())
				bGameThreadActive = false;

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
				swprintf_s(buffer_title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", sAppName.c_str(), 1.0f / fDeltaTime);
				SetConsoleTitleW(buffer_title);

				if (!OnUserUpdate(fDeltaTime))
					bGameThreadActive = false;

				// Handle Mouse Input - Check for window events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(hConsoleIn, &events);
				if (events > 0)
					ReadConsoleInputW(hConsoleIn, inBuf, events, &events);

				// Handle events - we only care about mouse clicks and movement
				// for now
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

	protected:
		CHAR_INFO* screen = nullptr;
		HANDLE hConsoleOut;
		HANDLE hConsoleIn;
		SMALL_RECT rectWindow;
		HWND hwnd;
		HDC hDC;
		std::wstring sAppName;
		std::wstring sFont;

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

	bool ConsolaProd::Focused()
	{
		return bFocused;
	}

	void ConsolaProd::FillRectangle(vi2d pos1, vi2d pos2, short c, short col)
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
		int x = 0;
		int y = radius;
		int p = 3 - 2 * radius;
		if (!radius) return;

		while (y >= x)
		{
			Draw({ pos.x - x, pos.y - y }, c, col);//upper left left
			Draw({ pos.x - y, pos.y - x }, c, col);//upper upper left
			Draw({ pos.x + y, pos.y - x }, c, col);//upper upper right
			Draw({ pos.x + x, pos.y - y }, c, col);//upper right right
			Draw({ pos.x - x, pos.y + y }, c, col);//lower left left
			Draw({ pos.x - y, pos.y + x }, c, col);//lower lower left
			Draw({ pos.x + y, pos.y + x }, c, col);//lower lower right
			Draw({ pos.x + x, pos.y + y }, c, col);//lower right right
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
		for (int i = radius; i != 0; i--)
		{
			DrawCircle(pos, i, c, col);
		}
	}

	void ConsolaProd::FillCircle(int x, int y, int radius, short c, short col)
	{
		FillCircle({ x, y }, radius, c, col);
	}

	void ConsolaProd::Draw(vi2d pos, short c, short col)
	{
		if (pos.x >= 0 && pos.x < nScreenWidth && pos.y >= 0 && pos.y < nScreenHeight)
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
		DrawLine(pos1, { pos2.x, pos1.y });
		DrawLine({ pos2.x, pos1.y }, pos2);
		DrawLine(pos2, { pos1.x, pos2.y });
		DrawLine({ pos1.x, pos2.y }, pos1);
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

	void ConsolaProd::DrawString(vi2d pos, std::wstring text, short c, short col)
	{
		if (pos.x > 0 && pos.y > 0 && pos.x <= nScreenWidth && pos.y <= nScreenHeight)
		{
			for (size_t i = 0; i < text.size(); i++)
			{
				screen[pos.y * nScreenWidth + pos.x + i].Char.UnicodeChar = text[i];
				screen[pos.y * nScreenWidth + pos.x + i].Attributes = col;
			}
		}
	}

	void ConsolaProd::DrawString(int x, int y, std::wstring text, short c, short col)
	{
		DrawString({ x, y }, text, c, col);
	}

	void ConsolaProd::Clear(short c, short col)
	{
		FillRectangle({ 0, 0 }, { nScreenWidth, nScreenHeight }, c, col);
	}

	vi2d ConsolaProd::GetMouse()
	{
		return { nMousePosX, nMousePosY };
	}

	int ConsolaProd::GetMouseX()
	{
		return nMousePosX;
	}

	int ConsolaProd::GetMouseY()
	{
		return nMousePosY;
	}

	vf2d ConsolaProd::GetMouseF()
	{
		return { (float)nMousePosX, (float)nMousePosY };
	}

	KeyState ConsolaProd::GetMouse(short button)
	{
		return mouse[button];
	}

	KeyState ConsolaProd::GetKey(short key)
	{
		return keys[key];
	}

	int ConsolaProd::GetScreenWidth()
	{
		return nScreenWidth;
	}

	int ConsolaProd::GetScreenHeight()
	{
		return nScreenHeight;
	}

	vi2d ConsolaProd::GetScreenSize()
	{
		return { nScreenWidth, nScreenHeight };
	}

	vf2d ConsolaProd::GetScreenSizeF()
	{
		return { (float)nScreenWidth, (float)nScreenHeight };
	}

	std::vector<int> ConsolaProd::AnyKeyHeld()
	{
		std::vector<int> output;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i].bHeld)
				output.push_back(i);
		}
		return output;
	}

	std::vector<int> ConsolaProd::AnyKeyPressed()
	{
		std::vector<int> output;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i].bPressed)
				output.push_back(i);
		}
		return output;
	}

	std::vector<int> ConsolaProd::AnyKeyReleased()
	{
		std::vector<int> output;
		for (int i = 0; i < keys.size(); i++)
		{
			if (keys[i].bReleased)
				output.push_back(i);
		}
		return output;
	}
}

#endif
