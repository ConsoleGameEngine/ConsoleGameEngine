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

#ifndef UNICODE
#error Enable Unicode in settings
#endif

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#pragma warning(disable : 4996)
#include <Windows.h>

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

typedef struct rcode
{
	bool ok;
	wchar_t* info;
} rcode;

const wchar_t* CONSOLAS = L"Consolas";
const wchar_t* COURIER_NEW = L"Courier New";
const wchar_t* LUCIDA_CONSOLE = L"Lucida Console";
const wchar_t* LUCIDA_SANS_TYPEWRITER = L"Lucida Sans Typewriter";
const wchar_t* MS_GOTHIC = L"MS Gothic";
const wchar_t* NSIMSUN = L"NSimSum";
const wchar_t* SIMSUM_EXTB = L"SimSum-Ext8";

typedef struct key_state
{
	bool held;
	bool released;
	bool pressed;
} key_state;

typedef struct sprite
{
	short* glyphs;
	short* colours;
	int width;
	int height;
} sprite;

void sprite_create(sprite* spr, int width, int height)
{
	if (width <= 0 || height <= 0)
		return;

	spr->width = width;
	spr->height = height;

	spr->glyphs = (short*)malloc(sizeof(short) * width * height);
	spr->colours = (short*)malloc(sizeof(short) * width * height);

	for (int i = 0; i < width * height; i++)
	{
		spr->glyphs[i] = L' ';
		spr->colours[i] = FG_BLACK;
	}
}

bool sprite_load(sprite* spr, const wchar_t* filename)
{
	spr->width = 0;
	spr->height = 0;

	FILE* f = NULL;
	_wfopen_s(&f, filename, L"rb");

	if (f == NULL)
		return false;

	fread(&spr->width, sizeof(int), 1, f);
	fread(&spr->height, sizeof(int), 1, f);

	sprite_create(spr, spr->width, spr->height);

	fread(spr->colours, sizeof(short), spr->width * spr->height, f);
	fread(spr->glyphs, sizeof(short), spr->width * spr->height, f);

	fclose(f);

	return true;
}

bool sprite_save(sprite* spr, const wchar_t* filename)
{
	FILE* f = NULL;
	_wfopen_s(&f, filename, L"wb");

	if (f == NULL)
		return false;

	fwrite(&spr->width, sizeof(int), 1, f);
	fwrite(&spr->height, sizeof(int), 1, f);

	fwrite(spr->colours, sizeof(short), spr->width * spr->height, f);
	fwrite(spr->glyphs, sizeof(short), spr->width * spr->height, f);

	fclose(f);

	return true;
}

sprite* sprite_new(const wchar_t* filename)
{
	sprite* spr = (sprite*)malloc(sizeof(sprite));
	if (!sprite_load(spr, filename))
		sprite_create(spr, 8, 8);

	return spr;
}

void sprite_set_glyph(sprite* spr, int x, int y, short c)
{
	if (x > 0 || x < spr->width || y > 0 || y < spr->height)
		spr->glyphs[y * spr->width + x] = c;
}

void sprite_set_colour(sprite* spr, int x, int y, short col)
{
	if (x > 0 || x < spr->width || y > 0 || y < spr->height)
		spr->colours[y * spr->width + x] = col;
}

short sprite_get_glyph(sprite* spr, int x, int y)
{
	if (x > 0 || x < spr->width || y > 0 || y < spr->height)
		return spr->glyphs[y * spr->width + x];

	return L' ';
}

short sprite_get_colour(sprite* spr, int x, int y)
{
	if (x > 0 || x < spr->width || y > 0 || y < spr->height)
		return spr->colours[y * spr->width + x];

	return FG_BLACK;
}

rcode rc_new(bool ok)
{
	rcode rc;
	rc.ok = ok;
	rc.info = (wchar_t*)malloc(sizeof(wchar_t) * 128);
	return rc;
}

typedef struct console_game_engine
{
	HANDLE console_out;
	HANDLE console_in;
	HWND wnd;
	HDC dc;
	wchar_t* app_name;
	wchar_t* font_name;
	CHAR_INFO* screen;
	SMALL_RECT rect_window;
	key_state keys[256];
	key_state mouse[5];
	short key_new_state[256];
	short key_old_state[256];
	bool mouse_new_state[5];
	bool mouse_old_state[5];
	int mouse_x;
	int mouse_y;
	int screen_width;
	int screen_height;
	int font_width;
	int font_height;
	float delta_time;
	bool game_active;
	bool focused;
	bool (*on_user_create)(void);
	bool (*on_user_update)(float);
} console_game_engine;

void cge_init(console_game_engine* cge, const wchar_t* app_name, const wchar_t* font_name)
{
	cge->console_out = GetStdHandle(STD_OUTPUT_HANDLE);
	cge->console_in = GetStdHandle(STD_INPUT_HANDLE);

	cge->wnd = GetConsoleWindow();
	cge->dc = GetDC(cge->wnd);
	cge->app_name = app_name;
	cge->font_name = font_name;
}

void cge_destroy(console_game_engine* cge)
{
	free(cge->screen);
}

void cge_construct_console(console_game_engine* cge, int screen_width, int screen_height, int font_width, int font_height)
{
	rcode rc = rc_new(false, L"");

	if (screen_width <= 0 || screen_height <= 0 || font_width <= 0 || font_height <= 0)
	{
		wcscpy(rc.info, L"Invalid width or height");
		return rc;
	}

	cge->screen_width = screen_width;
	cge->screen_height = screen_height;

	cge->font_width = font_width;
	cge->font_height = font_height;

	cge->console_out = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	if (cge->console_out == INVALID_HANDLE_VALUE)
	{
		wcscpy(rc.info, L"Can't create console screen buffer for output");
		return rc;
	}

	cge->rect_window.Left = 0;
	cge->rect_window.Top = 0;
	cge->rect_window.Right = 1;
	cge->rect_window.Bottom = 1;

	SetConsoleWindowInfo(cge->console_out, TRUE, &cge->rect_window);

	COORD coord;
	coord.X = cge->screen_width;
	coord.Y = cge->screen_height;

	if (!SetConsoleScreenBufferSize(cge->console_out, coord))
	{
		wcscpy(rc.info, L"Too large or to small screen width or height");
		return rc;
	}

	if (!SetConsoleActiveScreenBuffer(cge->console_out))
	{
		wcscpy(rc.info, L"Can't set console screen buffer");
		return rc;
	}

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = cge->font_width;
	cfi.dwFontSize.Y = cge->font_height;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy(cfi.FaceName, cge->font_name);
	if (!SetCurrentConsoleFontEx(cge->console_out, false, &cfi))
	{
		wcscpy(rc.info, L"Can't set font");
		return rc;
	}

	if (!SetConsoleMode(cge->console_in, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
	{
		wcscpy(rc.info, L"Could not set console mode (ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT)");
		return rc;
	}

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(cge->console_out, &csbi))
	{
		wcscpy(rc.info, L"Could not get console screen buffer info");
		return rc;
	}

	if (cge->screen_height > csbi.dwMaximumWindowSize.Y)
	{
		wcscpy(rc.info, L"Specified screen height larger than maximum window height");
		return rc;
	}

	if (cge->screen_width > csbi.dwMaximumWindowSize.X)
	{
		wcscpy(rc.info, L"Specified screen width larger than maximum window width");
		return rc;
	}

	cge->rect_window.Left = 0;
	cge->rect_window.Top = 0;
	cge->rect_window.Right = (short)(cge->screen_width - 1);
	cge->rect_window.Bottom = (short)(cge->screen_height - 1);

	SetConsoleWindowInfo(cge->console_out, TRUE, &cge->rect_window);
	cge->screen = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * cge->screen_width * cge->screen_height);
	memset(cge->screen, 0, sizeof(CHAR_INFO) * cge->screen_width * cge->screen_height);

	rc.ok = true;
	wcscpy(rc.info, L"Ok");
	return rc;
}

void cge_thread(console_game_engine* cge)
{
	if (!cge->on_user_create())
	{
		cge->game_active = false;
		return;
	}

	clock_t time_begin = 0;

	cge->mouse_x = -1;
	cge->mouse_y = -1;

	cge->delta_time = 0.0f;

	for (int i = 0; i < 256; i++)
	{
		cge->keys[i].held = false;
		cge->keys[i].pressed = false;
		cge->keys[i].released = false;
	}

	for (int i = 0; i < 5; i++)
	{
		cge->mouse[i].held = false;
		cge->mouse[i].pressed = false;
		cge->mouse[i].released = false;
	}

	while (cge->game_active)
	{
		time_begin = clock();

		wchar_t title[256];

		swprintf_s(title, 256, L"github.com/defini7 - Consola Prod - %s - FPS: %3.2f", cge->app_name, 1.0f / cge->delta_time);
		SetConsoleTitleW(title);

		if (!cge->on_user_update(cge->delta_time))
			cge->game_active = false;

		INPUT_RECORD buffer_in[32];
		DWORD events = 0;
		GetNumberOfConsoleInputEvents(cge->console_in, &events);
		if (events > 0)
			ReadConsoleInputW(cge->console_in, buffer_in, events, &events);

		for (DWORD i = 0; i < events; i++)
		{
			switch (buffer_in[i].EventType)
			{
			case FOCUS_EVENT:
				cge->focused = buffer_in[i].Event.FocusEvent.bSetFocus;
				break;

			case MOUSE_EVENT:
			{
				switch (buffer_in[i].Event.MouseEvent.dwEventFlags)
				{
				case MOUSE_MOVED:
				{
					cge->mouse_x = buffer_in[i].Event.MouseEvent.dwMousePosition.X;
					cge->mouse_y = buffer_in[i].Event.MouseEvent.dwMousePosition.Y;
				}
				break;

				case 0:
				{
					for (int m = 0; m < 5; m++)
						cge->mouse_new_state[m] = (buffer_in[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
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
			cge->key_new_state[i] = GetAsyncKeyState(i);

			cge->keys[i].pressed = false;
			cge->keys[i].released = false;

			if (cge->key_new_state[i] != cge->key_old_state[i])
			{
				if (cge->key_new_state[i] & 0x8000)
				{
					cge->keys[i].pressed = !cge->keys[i].held;
					cge->keys[i].held = true;
				}
				else
				{
					cge->keys[i].released = true;
					cge->keys[i].held = false;
				}
			}

			cge->key_old_state[i] = cge->key_new_state[i];
		}

		for (int m = 0; m < 5; m++)
		{
			cge->mouse[m].pressed = false;
			cge->mouse[m].released = false;

			if (cge->mouse_new_state[m] != cge->mouse_old_state[m])
			{
				if (cge->mouse_new_state[m])
				{
					cge->mouse[m].pressed = true;
					cge->mouse[m].held = true;
				}
				else
				{
					cge->mouse[m].released = true;
					cge->mouse[m].held = false;
				}
			}

			cge->mouse_old_state[m] = cge->mouse_new_state[m];
		}

		COORD size, coord;

		size.X = (short)cge->screen_width;
		size.Y = (short)cge->screen_height;

		coord.X = 0;
		coord.Y = 0;

		WriteConsoleOutputW(cge->console_out, cge->screen, size, coord, &cge->rect_window);
		
		cge->delta_time = (float)(clock() - time_begin) / (float)1000.0f;
	}

}

void cge_run(console_game_engine* cge, bool(*on_user_create)(void), bool(*on_user_update)(float))
{
	cge->game_active = true;
	cge->on_user_create = on_user_create;
	cge->on_user_update = on_user_update;
	cge_thread(cge);
}

bool cge_focused(console_game_engine* cge)
{
	return cge->focused;
}

void cge_draw(console_game_engine* cge, int x, int y, short c, short col)
{
	if (x >= 0 && x < cge->screen_width && y >= 0 && y < cge->screen_height)
	{
		cge->screen[y * cge->screen_width + x].Char.UnicodeChar = c;
		cge->screen[y * cge->screen_width + x].Attributes = col;
	}
}

void cge_fill_rectangle(console_game_engine* cge, int x, int y, int sx, int sy, short c, short col)
{
	for (int i = x; i <= x + sx; i++)
		for (int j = y; j <= y + sy; j++)
			cge_draw(cge, i, j, c, col);
}

void cge_draw_circle(console_game_engine* cge, int x, int y, int radius, short c, short col)
{
	if (!radius) return;

	int x1 = 0;
	int y1 = radius;
	int p = 3 - 2 * radius;

	while (y >= x)
	{
		cge_draw(cge, x - x1, y - y1, c, col);	// upper left left
		cge_draw(cge, x - y1, y - x1, c, col);	// upper upper left
		cge_draw(cge, x + y1, y - x1, c, col);	// upper upper right
		cge_draw(cge, x + x1, y - y1, c, col);	// upper right right
		cge_draw(cge, x - x1, y + y1, c, col);	// lower left left
		cge_draw(cge, x - y1, y + x1, c, col);	// lower lower left
		cge_draw(cge, x + y1, y + x1, c, col);	// lower lower right
		cge_draw(cge, x + x1, y + y1, c, col);	// lower right right

		if (p < 0)
		{
			p += 4 * x1 + 6;

			x1++;
		}
		else
		{
			p += 4 * (x1 - y1) + 10;

			x1++;
			y1--;
		}
	}
}

void _cge_draw_line(console_game_engine* cge, int sx, int ex, int ny, short c, short col)
{
	for (int i = sx; i <= ex; i++)
		cge_draw(cge, i, ny, c, col);
}

void cge_fill_circle(console_game_engine* cge, int x, int y, int radius, short c, short col)
{
	if (!radius) return;

	int x1 = 0;
	int y1 = radius;
	int p = 3 - 2 * radius;

	while (y1 >= x1)
	{
		_cge_draw_line(cge, x - x1, x + x1, y - y1, c, col);
		_cge_draw_line(cge, x - y1, x + y1, y - x1, c, col);
		_cge_draw_line(cge, x - x1, x + x1, y + y1, c, col);
		_cge_draw_line(cge, x - y1, x + y1, y + x1, c, col);
		if (p < 0) p += 4 * x1++ + 6;
		else p += 4 * (x1++ - y1--) + 10;
	}
}

void cge_draw_line(console_game_engine* cge, int x1, int y1, int x2, int y2, short c, short col)
{
	int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;

	dx = x2 - x1;
	dy = y2 - y1;

	dx1 = abs(dx);
	dy1 = abs(dy);

	px = 2 * dy1 - dx1;
	py = 2 * dx1 - dy1;

	if (dy1 <= dx1)
	{
		if (dx >= 0)
		{
			x = x1;
			y = x1;
			xe = x2;
		}
		else
		{
			x = x2;
			y = y2;
			xe = x1;
		}

		cge_draw(cge, x, y, c, col);

		for (i = 0; x < xe; i++)
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

			cge_draw(cge, x, y, c, col);
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

		cge_draw(cge, x, y, c, col);

		for (i = 0; y < ye; i++)
		{
			y = y + 1;

			if (py <= 0)
				py = py + 2 * dx1;
			else
			{
				if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
					x = x + 1;
				else
					x = x - 1;

				py = py + 2 * (dx1 - dy1);
			}

			cge_draw(cge, x, y, c, col);
		}
	}
}

void cge_draw_triangle(console_game_engine* cge, int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
{
	cge_draw_line(cge, x1, y1, x2, y2, c, col);
	cge_draw_line(cge, x2, y2, x3, y3, c, col);
	cge_draw_line(cge, x3, y3, x1, y1, c, col);
}

void _cge_swap(int* a, int* b)
{
	int temp = a;
	*a = *b;
	*b = temp;
}

// https://www.avrfreaks.net/sites/default/files/triangles.c
void cge_fill_triangle(console_game_engine* cge, int x1, int y1, int x2, int y2, int x3, int y3, short c, short col)
{
	int t1x, t2x, y, minx, maxx, t1xp, t2xp;

	bool changed1 = false;
	bool changed2 = false;

	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;

	// Sort vertices
	if (y1 > y2) { _cge_swap(&y1, &y2); _cge_swap(&x1, &x2); }
	if (y1 > y3) { _cge_swap(&y1, &y3); _cge_swap(&x1, &x3); }
	if (y2 > y3) { _cge_swap(&y2, &y3); _cge_swap(&x2, &x3); }

	t1x = t2x = x1; y = y1;   // Starting points
	dx1 = (int)(x2 - x1);

	if (dx1 < 0)
	{
		dx1 = -dx1;
		signx1 = -1;
	}
	else
		signx1 = 1;

	dy1 = (int)(y2 - y1);

	dx2 = (int)(x3 - x1);

	if (dx2 < 0)
	{
		dx2 = -dx2;
		signx2 = -1;
	}
	else
		signx2 = 1;

	dy2 = (int)(y3 - y1);

	if (dy1 > dx1)
	{   // swap values
		_cge_swap(&dx1, &dy1);
		changed1 = true;
	}
	if (dy2 > dx2)
	{   // swap values
		_cge_swap(&dy2, &dx2);
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

		_cge_draw_line(cge, minx, maxx, y, c, col);    // Draw line from min to max points found on the y
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
	dx1 = (int)(x3 - x2);

	if (dx1 < 0)
	{
		dx1 = -dx1;
		signx1 = -1;
	}
	else
		signx1 = 1;

	dy1 = (int)(y3 - y2);
	t1x = x2;

	if (dy1 > dx1)
	{   // swap values
		_cge_swap(&dy1, &dx1);
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

		_cge_draw_line(cge, minx, maxx, y, c, col);

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

void cge_draw_rectangle(console_game_engine* cge, int x, int y, int sx, int sy, short c, short col)
{
	for (int i = 0; i <= sx; i++)
	{
		cge_draw(cge, x + i, y, c, col);
		cge_draw(cge, x + i, y + sy, c, col);
	}

	for (int j = 0; j <= sy; j++)
	{
		cge_draw(cge, x, y + j, c, col);
		cge_draw(cge, x + sx, y + j, c, col);
	}
}

void cge_draw_sprite(console_game_engine* cge, int x, int y, sprite* spr)
{
	if (spr == NULL)
		return;

	for (int i = 0; i < spr->width; i++)
	{
		for (int j = 0; j < spr->height; j++)
		{
			if (sprite_get_glyph(spr, i, j) != L' ')
				cge_draw(cge, x + i, y + j, sprite_get_glyph(spr, i, j), sprite_get_colour(spr, i, j));
		}
	}
}

void cge_draw_partial_sprite(console_game_engine* cge, int x, int y, int fx, int fy, int fsx, int fsy, sprite* spr)
{
	if (spr == NULL || fx < 0 || fy < 0 || fx + fsx > spr->width || fy + fsy > spr->height)
		return;

	for (int i = x, k = 0; i < fx + fsx; i++, k++)
	{
		for (int j = y, m = 0; j < fy + fsy; j++, m++)
		{
			if (sprite_get_glyph(spr, i, j) != L' ')
				cge_draw(cge, x + k, y + m, sprite_get_glyph(spr, i, j), sprite_get_colour(spr, i, j));
		}
	}
}

typedef struct vec2d
{
	float x;
	float y;
} vec2d;

void cge_draw_wire_frame_model(console_game_engine* cge, vec2d* model_coordinates, int model_coordinates_amount, float x, float y, float r, float s, short c, short col)
{
	// Create translated model vector of coordinate pairs
	vec2d* transformed_coordinates = (vec2d*)malloc(sizeof(vec2d) * model_coordinates_amount);

	// Rotate
	for (int i = 0; i < model_coordinates_amount; i++)
	{
		transformed_coordinates[i].x = model_coordinates[i].x * cosf(r) - model_coordinates[i].y * sinf(r);
		transformed_coordinates[i].y = model_coordinates[i].x * sinf(r) + model_coordinates[i].y * cosf(r);
	}

	// Scale
	for (int i = 0; i < model_coordinates_amount; i++)
	{
		transformed_coordinates[i].x = model_coordinates[i].x * s;
		transformed_coordinates[i].y = model_coordinates[i].y * s;
	}

	// Translate
	for (int i = 0; i < model_coordinates_amount; i++)
	{
		transformed_coordinates[i].x = model_coordinates[i].x + x;
		transformed_coordinates[i].y = model_coordinates[i].y + y;
	}

	// Draw Closed Polygon
	for (int i = 0; i < model_coordinates_amount + 1; i++)
	{
		int j = (i + 1);
		cge_draw_line(cge, (int)transformed_coordinates[i % model_coordinates_amount].x, (int)transformed_coordinates[i % model_coordinates_amount].y,
			(int)transformed_coordinates[j % model_coordinates_amount].x, (int)transformed_coordinates[j % model_coordinates_amount].y, c, col);
	}

	free(transformed_coordinates);
}

void cge_draw_string(console_game_engine* cge, int x, int y, const wchar_t* text, int text_length, short col)
{
	for (size_t i = 0; i < text_length; i++)
	{
		cge->screen[y * cge->screen_width + x + i].Char.UnicodeChar = text[i];
		cge->screen[y * cge->screen_width + x + i].Attributes = col;
	}
}

void cge_clear(console_game_engine* cge, short c, short col)
{
	cge_fill_rectangle(cge, 0, 0, cge->screen_width - 1, cge->screen_height - 1, c, col);
}

int cge_get_mouse_x(console_game_engine* cge)
{
	return cge->mouse_x;
}

int cge_get_mouse_y(console_game_engine* cge)
{
	return cge->mouse_y;
}

key_state cge_get_mouse(console_game_engine* cge, short button)
{
	return cge->mouse[button];
}

key_state get_key(console_game_engine* cge, short key)
{
	return cge->keys[key];
}

int cge_get_screen_width(console_game_engine* cge)
{
	return cge->screen_width;
}

int GetScreenHeight(console_game_engine* cge)
{
	return cge->screen_height;
}

int GetCharacter(console_game_engine* cge, bool held, bool pressed, bool released)
{
	for (int i = ' '; i <= '~'; i++)
	{
		if (held)
		{
			if (cge->keys[i].held)
				return i;
		}
		else if (pressed)
		{
			if (cge->keys[i].pressed)
				return i;
		}
		else if (released)
		{
			if (cge->keys[i].released)
				return i;
		}

	}

	return -1;
}

float cge_get_delta_time(console_game_engine* cge)
{
	return cge->delta_time;
}
