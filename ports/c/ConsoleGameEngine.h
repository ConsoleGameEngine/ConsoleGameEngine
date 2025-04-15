#ifndef CONSOLE_GAME_ENGINE_H
#define CONSOLE_GAME_ENGINE_H

#pragma region consolegameengine_license
/***
*	BSD 3-Clause License

	Copyright (c) 2021 - 2025 Alex
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
	#include "ConsoleGameEngine.hpp"

	void cgeRun()
	{
		while (cgeUpdate())
		{
			for (int i = 0; i < cgeScreenWidth(); i++)
				for (int j = 0; j < cgeScreenHeight(); j++)
					cgeDraw(i, j, PIXEL_SOLID, rand() % 16);
		}
	}

	int main()
	{
		if (cgeConstructConsole(256, 240, 4, 4))
			cgeRun();

		return 0;
	}
**/
#pragma endregion

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef UNICODE
#pragma message("UNICODE is disabled, but let's try to enable it!")
#define UNICODE
#define _UNICODE
#endif

#include <Windows.h>
#include <math.h>
#include <stdio.h>

#pragma comment(lib, "winmm.lib")

#undef min
#undef max

typedef unsigned char cgeBool;

#define CGE_FALSE (cgeBool)0
#define CGE_TRUE (cgeBool)1

wchar_t* cgeUtilInitWchar(const wchar_t* pData);
void cgeUtilSwapInt(int* a, int* b);

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
} ForegroundColours;

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
} BackgroundColours;

typedef enum
{
	PIXEL_SOLID = 0x2588,
	PIXEL_THREEQUARTERS = 0x2593,
	PIXEL_HALF = 0x2592,
	PIXEL_QUARTER = 0x2591
} PixelType;

typedef enum
{
	CL_GRID_HORIZONTAL = 0x400,
	CL_GRID_LVERTICAL = 0x0800,
	CL_GRID_RVERTICAL = 0x1000,
	CL_UNDERSCORE = 0x8000
} CommonLvb;

typedef struct
{
	cgeBool bHeld;
	cgeBool bReleased;
	cgeBool bPressed;
} KeyState;

typedef struct Sprite
{
	int nWidth;
	int nHeight;

	wchar_t* pGlyphs;
	short* pColours;
} Sprite, *PSprite;

PSprite spriteNew();
void spriteDelete(PSprite pSprite);

PSprite spriteCreate(int nWidth, int nHeight);
PSprite spriteFromFile(const wchar_t* sFileName);

void spriteSetGlyph(PSprite pSprite, int x, int y, short nGlyph);
void spriteSetColour(PSprite pSprite, int x, int y, short nColour);

short spriteGetGlyph(PSprite pSprite, int x, int y);
short spriteGetColour(PSprite pSprite, int x, int y);

cgeBool spriteSave(PSprite pSprite, const wchar_t* sFileName);
cgeBool spriteLoad(PSprite pSprite, const wchar_t* sFileName);

typedef enum
{
	RC_OK,
	RC_INVALID_SCREEN_SIZE,
	RC_INVALID_SCREEN_BUFFER,
	RC_INVALID_FONT,
	RC_INVALID_CONSOLE_MODE,
	RC_INVALID_SCREEN_INFO,
} ErrorCode;

typedef struct ConsoleGameEngine
{
	wchar_t* sAppName;
	wchar_t* sFontName;

	CHAR_INFO* pScreen;
	HANDLE hConsoleOut;
	HANDLE hConsoleIn;

	SMALL_RECT rWindow;
	HWND hWindow;
	HDC hDrawContext;

	KeyState aryKeys[256];
	KeyState aryMouse[5];

	short aryKeyOldState[256];
	short aryKeyNewState[256];

	cgeBool aryMouseOldState[5];
	cgeBool aryMouseNewState[5];

	int nMouseX;
	int nMouseY;

	int nScreenWidth;
	int nScreenHeight;

	int nFontWidth;
	int nFontHeight;

	float fDeltaTime;

	cgeBool bIsFocused;
	cgeBool bIsGameActive;

	LARGE_INTEGER _tp1;
	LARGE_INTEGER _tp2;

} ConsoleGameEngine, *PConsoleGameEngine;

// !!! DO NOT access it directly !!!
static ConsoleGameEngine _engine;

void cgeInit();
void cgeShutdown();

BOOL WINAPI cgeHandlerRoutine(DWORD nEventCode);

ErrorCode cgeConstructConsole(int nScreenWidth, int nScreenHeight, int nFontWidth, int nFontHeight, const wchar_t* sTitle);
cgeBool cgeUpdate();

cgeBool cgeMakeSound(const wchar_t* sFileName, cgeBool bLooped);

void cgeDraw(int x, int y, wchar_t glyph, short col);
void cgeFillRectangle(int x, int y, int w, int h, wchar_t glyph, wchar_t col);
void cgeDrawHorizontalLine(int start, int end, int y, wchar_t glyph, short col);
void cgeDrawCircle(int x, int y, int r, wchar_t glyph, short col);
void cgeFillCircle(int x, int y, int r, wchar_t glyph, short col);
void cgeDrawLine(int x1, int y1, int x2, int y2, wchar_t glyph, short col);
void cgeDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col);
void cgeFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col);
void cgeDrawRectangle(int x, int y, int w, int h, wchar_t glyph, short col);
void cgeDrawSprite(int x, int y, const PSprite const pSprite);
void cgeDrawSpriteAlpha(int x, int y, const PSprite pSprite);
void cgeDrawPartialSprite(int x, int y, int fx, int fy, int fw, int fh, const PSprite const pSprite);
void cgeDrawPartialSpriteAlpha(int x, int y, int fx, int fy, int fw, int fh, const PSprite const pSprite);
void cgeDrawString(int x, int y, const wchar_t* sText, short col);
void cgeClear(wchar_t glyph, short col);

cgeBool cgeIsFocused();

int cgeGetMouseX();
int cgeGetMouseY();

KeyState cgeGetMouse(int nButton);
KeyState cgeGetKey(int nKey);

float cgeGetDeltaTime();

int cgeScreenWidth();
int cgeScreenHeight();

#endif