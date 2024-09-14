#include "ConsoleGameEngine.h"

wchar_t* utilInitWchar(const wchar_t* data)
{
	size_t size = (lstrlenW(data) + 1) * sizeof(wchar_t);

	wchar_t* new = (wchar_t*)malloc(size);
	memcpy(new, data, size);

	return new;
}

void utilSwapInt(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

Sprite spriteNew()
{
	return (Sprite)malloc(sizeof(_Sprite));
}

void spriteDelete(Sprite spr)
{
	if (spr)
	{
		free(spr->colours);
		free(spr->glyphs);

		spr->width = 0;
		spr->height = 0;
	}
}

Sprite spriteCreate(int width, int height)
{
	Sprite spr = spriteNew();

	spr->width = width;
	spr->height = height;

	size_t size = width * height * sizeof(short);

	spr->glyphs = (short*)malloc(size);
	spr->colours = (short*)malloc(size);

	// Compiler, SHUT UP!
	memset(spr->glyphs, L' ', size);
	memset(spr->colours, FG_BLACK, size);

	return spr;
}

Sprite spriteFromFile(const wchar_t* fileName)
{
	Sprite spr = spriteNew();
	
	if (spriteLoad(spr, fileName))
		return spr;

	return NULL;
}

void spriteSetGlyph(Sprite spr, int x, int y, short glyph)
{
	if (x >= 0 && x < spr->width && y >= 0 && y < spr->height)
		spr->glyphs[y * spr->width + x] = glyph;
}

void spriteSetColour(Sprite spr, int x, int y, short colour)
{
	if (x >= 0 && x < spr->width && y >= 0 && y < spr->height)
		spr->colours[y * spr->width + x] = colour;
}

short spriteGetGlyph(Sprite spr, int x, int y)
{
	if (x >= 0 && x < spr->width && y >= 0 && y < spr->height)
		return spr->glyphs[y * spr->width + x];

	return L' ';
}

short spriteGetColour(Sprite spr, int x, int y)
{
	if (x >= 0 && x < spr->width && y >= 0 && y < spr->height)
		return spr->colours[y * spr->width + x];

	return FG_BLACK;
}

bool spriteSave(Sprite spr, const wchar_t* fileName)
{
	if (!spr) return false;

	FILE* file = fopen((const char*)fileName, "wb");
	if (file == NULL) return false;

	fwrite(&spr->width, sizeof(int), 1, file);
	fwrite(&spr->height, sizeof(int), 1, file);

	size_t size = spr->width * spr->height;

	fwrite(spr->colours, sizeof(short), size, file);
	fwrite(spr->glyphs, sizeof(short), size, file);

	fclose(file);

	return true;
}

bool spriteLoad(Sprite spr, const wchar_t* fileName)
{
	if (!spr) return false;

	FILE* file = fopen((const char*)fileName, "rb");
	if (file == NULL) return false;

	fread(&spr->width, sizeof(int), 1, file);
	fread(&spr->height, sizeof(int), 1, file);

	size_t size = spr->width * spr->height;

	spr->glyphs = (short*)malloc(size * sizeof(short));
	spr->colours = (short*)malloc(size * sizeof(short));

	// Compiler, SHUT UP!
	fread(spr->colours, sizeof(short), size, file);
	fread(spr->glyphs, sizeof(short), size, file);

	fclose(file);

	return true;
}

ConsoleGameEngine cgeNew()
{
	ConsoleGameEngine cge = (ConsoleGameEngine)malloc(sizeof(_ConsoleGameEngine));

	// MSVC, SHUT UP!
	cge->consoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	cge->consoleIn = GetStdHandle(STD_INPUT_HANDLE);

	cge->window = GetConsoleWindow();
	cge->dc = GetDC(cge->window);

	cge->appName = utilInitWchar(L"Undefined");
	cge->fontName = utilInitWchar(L"Consolas");

	return cge;
}

void cgeDelete()
{
	free(_cge->screen);
}

BOOL WINAPI cgeHandlerRoutine(DWORD eventCode)
{
	if (eventCode == CTRL_CLOSE_EVENT)
	{
		_cge->isGameActive = false;
		return FALSE;
	}

	return TRUE;
}

RCODE cgeConstructConsole(int screenWidth, int screenHeight, int fontWidth, int fontHeight, const wchar_t* title)
{
	if (screenWidth <= 0 || screenHeight <= 0 || fontWidth <= 0 || fontHeight <= 0)
		return RC_INVALID_SCREEN_SIZE;

	_cge = cgeNew();

	_cge->screenWidth = screenWidth;
	_cge->screenHeight = screenHeight;

	_cge->fontWidth = fontWidth;
	_cge->fontHeight = fontHeight;

	_cge->consoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	if (_cge->consoleOut == INVALID_HANDLE_VALUE)
		return RC_INVALID_SCREEN_BUFFER;

	_cge->windowRect.Left = 0;
	_cge->windowRect.Top = 0;
	_cge->windowRect.Right = 1;
	_cge->windowRect.Bottom = 1;

	SetConsoleWindowInfo(_cge->consoleOut, TRUE, &_cge->windowRect);

	COORD coord = { (short)screenWidth, (short)screenHeight };

	if (!SetConsoleScreenBufferSize(_cge->consoleOut, coord)) return RC_INVALID_SCREEN_SIZE;
	if (!SetConsoleActiveScreenBuffer(_cge->consoleOut)) return RC_INVALID_SCREEN_BUFFER;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = fontWidth;
	cfi.dwFontSize.Y = fontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy(cfi.FaceName, _cge->fontName);
	if (!SetCurrentConsoleFontEx(_cge->consoleOut, false, &cfi))
		return RC_INVALID_FONT;

	if (!SetConsoleMode(_cge->consoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return RC_INVALID_CONSOLE_MODE;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(_cge->consoleOut, &csbi))
		return RC_INVALID_SCREEN_INFO;

	if (screenHeight > csbi.dwMaximumWindowSize.Y)
		return RC_INVALID_SCREEN_SIZE;

	if (screenWidth > csbi.dwMaximumWindowSize.X)
		return RC_INVALID_SCREEN_SIZE;

	_cge->windowRect.Left = 0;
	_cge->windowRect.Top = 0;
	_cge->windowRect.Right = screenWidth - 1;
	_cge->windowRect.Bottom = screenHeight - 1;

	SetConsoleWindowInfo(_cge->consoleOut, TRUE, &_cge->windowRect);

	_cge->screen = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * screenWidth * screenHeight);
	memset(_cge->screen, 0, sizeof(CHAR_INFO) * screenWidth * screenHeight);

	_cge->isGameActive = true;

	QueryPerformanceCounter(&_cge->_tp1);
	QueryPerformanceCounter(&_cge->_tp2);

	for (int i = 0; i < 256; i++)
	{
		_cge->keys[i].held = false;
		_cge->keys[i].pressed = false;
		_cge->keys[i].released = false;
	}

	for (int i = 0; i < 5; i++)
	{
		_cge->mouse[i].held = false;
		_cge->mouse[i].pressed = false;
		_cge->mouse[i].released = false;
	}

	SetConsoleCtrlHandler(cgeHandlerRoutine, TRUE);

	return RC_OK;
}

RCODE cgeUpdate()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&_cge->_tp2);
	_cge->deltaTime = (float)(_cge->_tp2.QuadPart - _cge->_tp1.QuadPart) / (float)freq.QuadPart;
	_cge->_tp1 = _cge->_tp2;

	wchar_t title[256];
	swprintf_s(title, 256, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", _cge->appName, 1.0f / _cge->deltaTime);
	SetConsoleTitleW(title);

	INPUT_RECORD inBuf[32];
	DWORD events = 0;

	GetNumberOfConsoleInputEvents(_cge->consoleIn, &events);

	if (events > 0)
		ReadConsoleInputW(_cge->consoleIn, inBuf, events, &events);

	for (DWORD i = 0; i < events; i++)
	{
		switch (inBuf[i].EventType)
		{
		case FOCUS_EVENT:
			_cge->isFocused = inBuf[i].Event.FocusEvent.bSetFocus;
			break;

		case WINDOW_BUFFER_SIZE_EVENT:
		{
			_cge->screenWidth = (int)(inBuf[i].Event.WindowBufferSizeEvent.dwSize.X);
			_cge->screenHeight = (int)(inBuf[i].Event.WindowBufferSizeEvent.dwSize.Y);
		}
		break;

		case MOUSE_EVENT:
		{
			switch (inBuf[i].Event.MouseEvent.dwEventFlags)
			{
			case MOUSE_MOVED:
			{
				_cge->mouseX = (int)(inBuf[i].Event.MouseEvent.dwMousePosition.X);
				_cge->mouseY = (int)(inBuf[i].Event.MouseEvent.dwMousePosition.Y);
			}
			break;

			case 0:
			{
				for (int m = 0; m < 5; m++)
					_cge->mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
			}
			break;

			default:
				break;
			}
		}
		break;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		_cge->keyNewState[i] = GetAsyncKeyState(i);

		_cge->keys[i].pressed = false;
		_cge->keys[i].released = false;

		if (_cge->keyNewState[i] != _cge->keyOldState[i])
		{
			if (_cge->keyNewState[i] & 0x8000)
			{
				_cge->keys[i].pressed = !_cge->keys[i].held;
				_cge->keys[i].held = true;
			}
			else
			{
				_cge->keys[i].released = true;
				_cge->keys[i].held = false;
			}
		}

		_cge->keyOldState[i] = _cge->keyNewState[i];
	}

	for (int i = 0; i < 5; i++)
	{
		_cge->mouse[i].pressed = false;
		_cge->mouse[i].released = false;

		if (_cge->mouseNewState[i] != _cge->mouseOldState[i])
		{
			if (_cge->mouseNewState[i])
			{
				_cge->mouse[i].pressed = true;
				_cge->mouse[i].held = true;
			}
			else
			{
				_cge->mouse[i].released = true;
				_cge->mouse[i].held = false;
			}
		}

		_cge->mouseOldState[i] = _cge->mouseNewState[i];
	}

	COORD size = { (short)_cge->screenWidth, (short)_cge->screenHeight };
	COORD coord = { 0, 0 };

	WriteConsoleOutputW(_cge->consoleOut, _cge->screen, size, coord, &_cge->windowRect);

	return _cge->isGameActive;
}

bool cgeMakeSound(const wchar_t* fileName, bool looped)
{
	DWORD flags = SND_ASYNC | SND_FILENAME;
	if (looped) flags |= SND_LOOP;

	return PlaySoundW(fileName, NULL, flags);
}

void cgeDraw(int x, int y, wchar_t glyph, short col)
{
	if (x >= 0 && x < _cge->screenWidth && y >= 0 && y < _cge->screenHeight)
	{
		_cge->screen[y * _cge->screenWidth + x].Char.UnicodeChar = glyph;
		_cge->screen[y * _cge->screenWidth + x].Attributes = col;
	}
}

void cgeFillRectangle(int x, int y, int sizeX, int sizeY, wchar_t glyph, wchar_t col)
{
	for (int i = 0; i < sizeX; i++)
		for (int j = 0; j < sizeY; j++)
			cgeDraw(x + i, y + j, glyph, col);
}

void cgeDrawHorizontalLine(int start, int end, int y, wchar_t glyph, short col)
{
	for (int i = start; i <= end; i++)
		cgeDraw(i, y, glyph, col);
}

void cgeDrawCircle(int x, int y, int radius, wchar_t glyph, short col)
{
	if (radius <= 0) return;

	int x1 = 0;
	int y1 = radius;
	int p = 3 - 2 * radius;

	while (y1 >= x1)
	{
		cgeDraw(x - x1, y - y1, glyph, col);
		cgeDraw(x - y1, y - x1, glyph, col);
		cgeDraw(x + y1, y - x1, glyph, col);
		cgeDraw(x + x1, y - y1, glyph, col);
		cgeDraw(x - x1, y + y1, glyph, col);
		cgeDraw(x - y1, y + x1, glyph, col);
		cgeDraw(x + y1, y + x1, glyph, col);
		cgeDraw(x + x1, y + y1, glyph, col);

		if (p < 0) p += 4 * x1++ + 6;
		else p += 4 * (x1++ - y1--) + 10;
	}
}

void cgeFillCircle(int x, int y, int radius, wchar_t glyph, short col)
{
	if (radius <= 0) return;

	int x1 = 0;
	int y1 = radius;
	int p = 3 - 2 * radius;

	while (y1 >= x1)
	{
		cgeDrawHorizontalLine(x - x1, x + x1, y - y1, glyph, col);
		cgeDrawHorizontalLine(x - y1, x + y1, y - x1, glyph, col);
		cgeDrawHorizontalLine(x - x1, x + x1, y + y1, glyph, col);
		cgeDrawHorizontalLine(x - y1, x + y1, y + x1, glyph, col);

		if (p < 0) p += 4 * x1++ + 6;
		else p += 4 * (x1++ - y1--) + 10;
	}
}

void cgeDrawLine(int x1, int y1, int x2, int y2, wchar_t glyph, short col)
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

		cgeDraw(x, y, glyph, col);

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

			cgeDraw(x, y, glyph, col);
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

		cgeDraw(x, y, glyph, col);

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

			cgeDraw(x, y, glyph, col);
		}
	}
}

void cgeDrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col)
{
	cgeDrawLine(x1, y1, x2, y2, glyph, col);
	cgeDrawLine(x2, y2, x3, y3, glyph, col);
	cgeDrawLine(x3, y3, x1, y1, glyph, col);
}

void cgeFillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, wchar_t glyph, short col)
{
	int t1x, t2x, y, minx, maxx, t1xp, t2xp;

	bool changed1 = false;
	bool changed2 = false;

	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;

	if (y1 > y2) { utilSwapInt(&y1, &y2); utilSwapInt(&x1, &x2); }
	if (y1 > y3) { utilSwapInt(&y1, &y3); utilSwapInt(&x1, &x3); }
	if (y2 > y3) { utilSwapInt(&y2, &y3); utilSwapInt(&x2, &x3); }

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
		utilSwapInt(&dx1, &dy1);
		changed1 = true;
	}

	if (dy2 > dx2)
	{
		utilSwapInt(&dy2, &dx2);
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

		cgeDrawHorizontalLine(minx, maxx, y, glyph, col);

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
		utilSwapInt(&dy1, &dx1);
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

		cgeDrawHorizontalLine(minx, maxx, y, glyph, col);

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

void cgeDrawRectangle(int x, int y, int sizeX, int sizeY, wchar_t glyph, short col)
{
	for (int i = 0; i <= sizeX; i++)
	{
		cgeDraw(x + i, y, glyph, col);
		cgeDraw(x + i, y + sizeY, glyph, col);
	}

	for (int j = 0; j <= sizeY; j++)
	{
		cgeDraw(x, y + j, glyph, col);
		cgeDraw(x + sizeX, y + j, glyph, col);
	}
}

void cgeDrawSprite(int x, int y, Sprite sprite)
{
	if (sprite == NULL) return;

	for (int i = 0; i < sprite->width; i++)
		for (int j = 0; j < sprite->height; j++)
		{
			short glyph = spriteGetGlyph(sprite, i, j);
			short col = spriteGetColour(sprite, i, j);

			cgeDraw(x + i, y + j, glyph, col | col * 16);
		}
}

void cgeDrawSpriteAlpha(int x, int y, Sprite sprite)
{
	if (sprite == NULL) return;

	for (int i = 0; i < sprite->width; i++)
		for (int j = 0; j < sprite->height; j++)
		{
			short glyph = spriteGetGlyph(sprite, i, j);

			if (glyph != L' ')
			{
				short col = spriteGetColour(sprite, i, j);
				cgeDraw(x + i, y + j, glyph, col | col * 16);
			}
		}
}

void cgeDrawPartialSprite(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite sprite)
{
	if (sprite == NULL) return;

	for (int i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
		for (int j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
		{
			short glyph = spriteGetGlyph(sprite, i, j);
			short col = spriteGetColour(sprite, i, j);

			cgeDraw(x + x1, y + y1, glyph, col | col * 16);
		}
}

void cgeDrawPartialSpriteAlpha(int x, int y, int fx, int fy, int fsizeX, int fsizeY, Sprite sprite)
{
	if (sprite == NULL) return;

	for (int i = fx, x1 = 0; i < fx + fsizeX; i++, x1++)
		for (int j = fy, y1 = 0; j < fy + fsizeY; j++, y1++)
		{
			short glyph = spriteGetGlyph(sprite, i, j);

			if (glyph != L' ')
			{
				short col = spriteGetColour(sprite, i, j);
				cgeDraw(x + x1, y + y1, glyph, col | col * 16);
			}
		}
}

void cgeDrawString(int x, int y, const wchar_t* text, short col)
{
	size_t len = lstrlenW(text);

	if (x + len < _cge->screenWidth && x >= 0 && y >= 0 && y < _cge->screenHeight)
	{
		for (size_t i = 0; i < len; i++)
		{
			_cge->screen[y * _cge->screenWidth + x + i].Char.UnicodeChar = text[i];
			_cge->screen[y * _cge->screenWidth + x + i].Attributes = col;
		}
	}
}

void cgeClear(wchar_t glyph, short col)
{
	cgeFillRectangle(0, 0, _cge->screenWidth, _cge->screenHeight, glyph, col);
}

bool cgeIsFocused()
{
	return _cge->isFocused;
}

int cgeGetMouseX()
{
	return _cge->mouseX;
}

int cgeGetMouseY()
{
	return _cge->mouseY;
}

KeyState cgeGetMouse(int button)
{
	return _cge->mouse[button];
}

KeyState cgeGetKey(int key)
{
	return _cge->keys[key];
}

float cgeGetDeltaTime()
{
	return _cge->deltaTime;
}

int cgeScreenWidth()
{
	return _cge->screenWidth;
}

int cgeScreenHeight()
{
	return _cge->screenHeight;
}
