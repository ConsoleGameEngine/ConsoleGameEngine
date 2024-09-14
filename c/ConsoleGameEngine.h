#ifndef CONSOLE_GAME_ENGINE_H
#define CONSOLE_GAME_ENGINE_H

#pragma region consolegameengine_license
/***
*	BSD 3-Clause License

	Copyright (c) 2021 - 2024 Alex
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
	#define CGE_IMPL
	#include "ConsoleGameEngine.hpp"

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

#define _CRT_SECURE_NO_WARNINGS

#ifndef UNICODE
#pragma message("UNICODE is disabled, but let's try to enable it!")
#define UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

#undef min
#undef max

wchar_t* utilInitWchar(const wchar_t* data);
void utilSwapInt(int* a, int* b);

typedef enum
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
} FG_COLORS;

typedef enum
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
} BG_COLORS;

typedef enum
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
} PIXEL_TYPE;

typedef enum
{
	CL_GRID_HORIZONTAL = 0x400,
	CL_GRID_LVERTICAL = 0x0800,
	CL_GRID_RVERTICAL = 0x1000,
	CL_UNDERSCORE = 0x8000
} COMMON_LVB;

typedef struct
{
	bool held;
	bool released;
	bool pressed;
} KeyState;

typedef struct
{
	int width;
	int height;

	short* glyphs;
	short* colours;
} _Sprite, *Sprite;

Sprite spriteNew();
void spriteDelete(Sprite spr);

Sprite spriteCreate(int width, int height);
Sprite spriteFromFile(const wchar_t* fileName);

void spriteSetGlyph(Sprite spr, int x, int y, short glyph);
void spriteSetColour(Sprite spr, int x, int y, short colour);

short spriteGetGlyph(Sprite spr, int x, int y);
short spriteGetColour(Sprite spr, int x, int y);

bool spriteSave(Sprite spr, const wchar_t* fileName);
bool spriteLoad(Sprite spr, const wchar_t* fileName);

typedef enum
{
	RC_OK,
	RC_INVALID_SCREEN_SIZE,
	RC_INVALID_SCREEN_BUFFER,
	RC_INVALID_FONT,
	RC_INVALID_CONSOLE_MODE,
	RC_INVALID_SCREEN_INFO,
} RCODE;

typedef struct
{
	wchar_t* appName;
	wchar_t* fontName;

	CHAR_INFO* screen;
	HANDLE consoleOut;
	HANDLE consoleIn;

	SMALL_RECT windowRect;
	HWND window;
	HDC dc;

	KeyState keys[256];
	KeyState mouse[5];

	short keyOldState[256];
	short keyNewState[256];

	bool mouseOldState[5];
	bool mouseNewState[5];

	int mouseX;
	int mouseY;

	int screenWidth;
	int screenHeight;

	int fontWidth;
	int fontHeight;

	float deltaTime;

	bool isFocused;
	bool isGameActive;

	LARGE_INTEGER _tp1;
	LARGE_INTEGER _tp2;

} _ConsoleGameEngine, *ConsoleGameEngine;

ConsoleGameEngine _cge;

ConsoleGameEngine cgeNew();
void cgeDelete();

BOOL WINAPI cgeHandlerRoutine(DWORD eventCode);

RCODE cgeConstructConsole(int screenWidth, int screenHeight, int fontWidth, int fontHeight, const wchar_t* title);
RCODE cgeUpdate();

bool cgeMakeSound(const wchar_t* fileName, bool looped);

void cgeDraw(int x, int y, wchar_t glyph, short col);
void cgeFillRectangle(int x, int y, int sizeX, int sizeY, wchar_t glyph, wchar_t col);
void cgeDrawHorizontalLine(int start, int end, int y, wchar_t glyph, short col);
void cgeDrawCircle(int x, int y, int radius, wchar_t glyph, short col);
void cgeFillCircle(int x, int y, int radius, wchar_t glyph, short col);
void cgeDrawLine(int x1, int y1, int x2, int y2, wchar_t glyph, short col);
void cgeDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col);
void cgeFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col);
void cgeDrawRectangle(int x, int y, int sizeX, int sizeY, wchar_t glyph, short col);
void cgeDrawSprite(int x, int y, Sprite sprite);
void cgeDrawSpriteAlpha(int x, int y, Sprite sprite);
void cgeDrawPartialSprite(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite sprite);
void cgeDrawPartialSpriteAlpha(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite sprite);
void cgeDrawString(int x, int y, const wchar_t* text, short col);
void cgeClear(wchar_t glyph, short col);

bool cgeIsFocused();

int cgeGetMouseX();
int cgeGetMouseY();

KeyState cgeGetMouse(int button);
KeyState cgeGetKey(int key);

float cgeGetDeltaTime();

int cgeScreenWidth();
int cgeScreenHeight();

#endif
