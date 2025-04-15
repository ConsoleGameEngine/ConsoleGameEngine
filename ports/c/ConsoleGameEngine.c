#include "ConsoleGameEngine.h"

wchar_t* cgeUtilInitWchar(const wchar_t* pData)
{
	size_t nBytes = (lstrlenW(pData) + 1) * sizeof(wchar_t);

	wchar_t* pNew = (wchar_t*)malloc(nBytes);
	memcpy(pNew, pData, nBytes);

	return pNew;
}

void cgeUtilSwapInt(int* a, int* b)
{
	int nTemp = *a;
	*a = *b;
	*b = nTemp;
}

PSprite spriteNew()
{
	return (PSprite)malloc(sizeof(Sprite));
}

void spriteDelete(PSprite pSprite)
{
	if (pSprite)
	{
		free(pSprite->pColours);
		free(pSprite->pGlyphs);
		free(pSprite);
	}
}

PSprite spriteCreate(int nWidth, int nHeight)
{
	PSprite pSprite = spriteNew();

	pSprite->nWidth = nWidth;
	pSprite->nHeight = nHeight;

	size_t nSize = nWidth * nHeight;

	pSprite->pGlyphs = (wchar_t*)malloc(nSize * sizeof(wchar_t));
	pSprite->pColours = (short*)malloc(nSize * sizeof(short));

	memset(pSprite->pGlyphs, L' ', nSize * sizeof(wchar_t));
	memset(pSprite->pColours, FG_BLACK, nSize * sizeof(short));

	return pSprite;
}

PSprite spriteFromFile(const wchar_t* sFileName)
{
	PSprite pSprite = spriteNew();

	if (spriteLoad(pSprite, sFileName))
		return pSprite;

	return NULL;
}

void spriteSetGlyph(PSprite pSprite, int x, int y, short nGlyph)
{
	if (x >= 0 && x < pSprite->nWidth && y >= 0 && y < pSprite->nHeight)
		pSprite->pGlyphs[y * pSprite->nWidth + x] = nGlyph;
}

void spriteSetColour(PSprite pSprite, int x, int y, short nColour)
{
	if (x >= 0 && x < pSprite->nWidth && y >= 0 && y < pSprite->nHeight)
		pSprite->pColours[y * pSprite->nWidth + x] = nColour;
}

short spriteGetGlyph(const PSprite pSprite, int x, int y)
{
	if (x >= 0 && x < pSprite->nWidth && y >= 0 && y < pSprite->nHeight)
		return pSprite->pGlyphs[y * pSprite->nWidth + x];

	return L' ';
}

short spriteGetColour(const PSprite pSprite, int x, int y)
{
	if (x >= 0 && x < pSprite->nWidth && y >= 0 && y < pSprite->nHeight)
		return pSprite->pColours[y * pSprite->nWidth + x];

	return FG_BLACK;
}

cgeBool spriteSave(const PSprite pSprite, const wchar_t* sFileName)
{
	if (!pSprite)
		return CGE_FALSE;

	FILE* pFile;
	_wfopen_s(&pFile, sFileName, L"wb");

	if (!pFile)
		return CGE_FALSE;

	fwrite(&pSprite->nWidth, sizeof(int), 1, pFile);
	fwrite(&pSprite->nHeight, sizeof(int), 1, pFile);

	size_t nSize = pSprite->nWidth * pSprite->nHeight;

	fwrite(pSprite->pColours, sizeof(short), nSize, pFile);
	fwrite(pSprite->pGlyphs, sizeof(short), nSize, pFile);

	fclose(pFile);

	return CGE_TRUE;
}

cgeBool spriteLoad(PSprite pSprite, const wchar_t* sFileName)
{
	if (!pSprite)
		return CGE_FALSE;

	FILE* pFile;
	_wfopen_s(&pFile, sFileName, L"rb");

	if (!pFile)
		return CGE_FALSE;

	fread(&pSprite->nWidth, sizeof(int), 1, pFile);
	fread(&pSprite->nHeight, sizeof(int), 1, pFile);

	size_t nSize = pSprite->nWidth * pSprite->nHeight;

	pSprite->pGlyphs = (short*)malloc(nSize * sizeof(short));
	pSprite->pColours = (short*)malloc(nSize * sizeof(short));

	fread(pSprite->pColours, sizeof(short), nSize, pFile);
	fread(pSprite->pGlyphs, sizeof(short), nSize, pFile);

	fclose(pFile);

	return CGE_TRUE;
}

void cgeInit()
{
	_engine.hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
	_engine.hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

	_engine.hWindow = GetConsoleWindow();
	_engine.hDrawContext = GetDC(_engine.hWindow);

	_engine.sAppName = cgeUtilInitWchar(L"Undefined");
	_engine.sFontName = cgeUtilInitWchar(L"Consolas");
}

void cgeShutdown()
{
	free(_engine.sAppName);
	free(_engine.sFontName);
	free(_engine.pScreen);
}

BOOL WINAPI cgeHandlerRoutine(DWORD eventCode)
{
	if (eventCode == CTRL_CLOSE_EVENT)
	{
		_engine.bIsGameActive = CGE_FALSE;
		return FALSE;
	}

	return TRUE;
}

ErrorCode cgeConstructConsole(int nScreenWidth, int nScreenHeight, int nFontWidth, int nFontHeight, const wchar_t* sTitle)
{
	if (nScreenWidth <= 0 || nScreenHeight <= 0 || nFontWidth <= 0 || nFontHeight <= 0)
		return RC_INVALID_SCREEN_SIZE;

	cgeInit();

	_engine.nScreenWidth = nScreenWidth;
	_engine.nScreenHeight = nScreenHeight;

	_engine.nFontWidth = nFontWidth;
	_engine.nFontHeight = nFontHeight;

	_engine.hConsoleOut = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	if (_engine.hConsoleOut == INVALID_HANDLE_VALUE)
		return RC_INVALID_SCREEN_BUFFER;

	_engine.rWindow.Left = 0;
	_engine.rWindow.Top = 0;
	_engine.rWindow.Right = 1;
	_engine.rWindow.Bottom = 1;

	SetConsoleWindowInfo(_engine.hConsoleOut, TRUE, &_engine.rWindow);

	COORD coord = { (short)nScreenWidth, (short)nScreenHeight };

	if (!SetConsoleScreenBufferSize(_engine.hConsoleOut, coord))
		return RC_INVALID_SCREEN_SIZE;

	if (!SetConsoleActiveScreenBuffer(_engine.hConsoleOut))
		return RC_INVALID_SCREEN_BUFFER;

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = nFontWidth;
	cfi.dwFontSize.Y = nFontHeight;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;

	wcscpy(cfi.FaceName, _engine.sFontName);

	if (!SetCurrentConsoleFontEx(_engine.hConsoleOut, FALSE, &cfi))
		return RC_INVALID_FONT;

	if (!SetConsoleMode(_engine.hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
		return RC_INVALID_CONSOLE_MODE;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (!GetConsoleScreenBufferInfo(_engine.hConsoleOut, &csbi))
		return RC_INVALID_SCREEN_INFO;

	if (nScreenHeight > csbi.dwMaximumWindowSize.Y)
		return RC_INVALID_SCREEN_SIZE;

	if (nScreenWidth > csbi.dwMaximumWindowSize.X)
		return RC_INVALID_SCREEN_SIZE;

	_engine.rWindow.Left = 0;
	_engine.rWindow.Top = 0;
	_engine.rWindow.Right = nScreenWidth - 1;
	_engine.rWindow.Bottom = nScreenHeight - 1;

	SetConsoleWindowInfo(_engine.hConsoleOut, TRUE, &_engine.rWindow);

	_engine.pScreen = (CHAR_INFO*)calloc(nScreenWidth * nScreenHeight, sizeof(CHAR_INFO));

	_engine.bIsGameActive = CGE_TRUE;

	QueryPerformanceCounter(&_engine._tp1);
	QueryPerformanceCounter(&_engine._tp2);

	for (int i = 0; i < 256; i++)
	{
		_engine.aryKeys[i].bHeld = CGE_FALSE;
		_engine.aryKeys[i].bPressed = CGE_FALSE;
		_engine.aryKeys[i].bReleased = CGE_FALSE;
	}

	for (int i = 0; i < 5; i++)
	{
		_engine.aryMouse[i].bHeld = CGE_FALSE;
		_engine.aryMouse[i].bPressed = CGE_FALSE;
		_engine.aryMouse[i].bReleased = CGE_FALSE;
	}

	SetConsoleCtrlHandler(cgeHandlerRoutine, TRUE);

	return RC_OK;
}

cgeBool cgeUpdate()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&_engine._tp2);
	_engine.fDeltaTime = (float)(_engine._tp2.QuadPart - _engine._tp1.QuadPart) / (float)freq.QuadPart;
	_engine._tp1 = _engine._tp2;

	wchar_t sTitle[128];
	swprintf_s(sTitle, 128, L"github.com/defini7 - Console Game Engine - %s - FPS: %3.2f", _engine.sAppName, 1.0f / _engine.fDeltaTime);
	SetConsoleTitleW(sTitle);

	INPUT_RECORD irBuffer[32];
	DWORD nEvents = 0;

	GetNumberOfConsoleInputEvents(_engine.hConsoleIn, &nEvents);

	if (nEvents > 0)
		ReadConsoleInputW(_engine.hConsoleIn, irBuffer, nEvents, &nEvents);

	for (DWORD i = 0; i < nEvents; i++)
	{
		switch (irBuffer[i].EventType)
		{
		case FOCUS_EVENT:
			_engine.bIsFocused = irBuffer[i].Event.FocusEvent.bSetFocus;
		break;

		case WINDOW_BUFFER_SIZE_EVENT:
		{
			_engine.nScreenWidth = (int)(irBuffer[i].Event.WindowBufferSizeEvent.dwSize.X);
			_engine.nScreenHeight = (int)(irBuffer[i].Event.WindowBufferSizeEvent.dwSize.Y);
		}
		break;

		case MOUSE_EVENT:
		{
			switch (irBuffer[i].Event.MouseEvent.dwEventFlags)
			{
			case MOUSE_MOVED:
			{
				_engine.nMouseX = (int)(irBuffer[i].Event.MouseEvent.dwMousePosition.X);
				_engine.nMouseY = (int)(irBuffer[i].Event.MouseEvent.dwMousePosition.Y);
			}
			break;

			case 0:
			{
				for (int m = 0; m < 5; m++)
					_engine.aryMouseNewState[m] = (irBuffer[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;
			}
			break;
			}
		}
		break;
		}
	}

	for (int i = 0; i < 256; i++)
	{
		_engine.aryKeyNewState[i] = GetAsyncKeyState(i);

		_engine.aryKeys[i].bPressed = CGE_FALSE;
		_engine.aryKeys[i].bReleased = CGE_FALSE;

		if (_engine.aryKeyNewState[i] != _engine.aryKeyOldState[i])
		{
			if (_engine.aryKeyNewState[i] & 0x8000)
			{
				_engine.aryKeys[i].bPressed = !_engine.aryKeys[i].bHeld;
				_engine.aryKeys[i].bHeld = CGE_TRUE;
			}
			else
			{
				_engine.aryKeys[i].bReleased = CGE_TRUE;
				_engine.aryKeys[i].bHeld = CGE_FALSE;
			}
		}

		_engine.aryKeyOldState[i] = _engine.aryKeyNewState[i];
	}

	for (int i = 0; i < 5; i++)
	{
		_engine.aryMouse[i].bPressed = CGE_FALSE;
		_engine.aryMouse[i].bReleased = CGE_FALSE;

		if (_engine.aryMouseNewState[i] != _engine.aryMouseOldState[i])
		{
			if (_engine.aryMouseNewState[i])
			{
				_engine.aryMouse[i].bPressed = CGE_TRUE;
				_engine.aryMouse[i].bHeld = CGE_TRUE;
			}
			else
			{
				_engine.aryMouse[i].bReleased = CGE_TRUE;
				_engine.aryMouse[i].bHeld = CGE_FALSE;
			}
		}

		_engine.aryMouseOldState[i] = _engine.aryMouseNewState[i];
	}

	COORD size = { (short)_engine.nScreenWidth, (short)_engine.nScreenHeight };
	COORD coord = { 0, 0 };

	WriteConsoleOutputW(_engine.hConsoleOut, _engine.pScreen, size, coord, &_engine.rWindow);

	return _engine.bIsGameActive;
}

cgeBool cgeMakeSound(const wchar_t* sFileName, cgeBool bLooped)
{
	DWORD nFlags = SND_ASYNC | SND_FILENAME;
	if (bLooped) nFlags |= SND_LOOP;

	return PlaySoundW(sFileName, NULL, nFlags);
}

void cgeDraw(int x, int y, wchar_t glyph, short col)
{
	if (x >= 0 && x < _engine.nScreenWidth && y >= 0 && y < _engine.nScreenHeight)
	{
		_engine.pScreen[y * _engine.nScreenWidth + x].Char.UnicodeChar = glyph;
		_engine.pScreen[y * _engine.nScreenWidth + x].Attributes = col;
	}
}

void cgeFillRectangle(int x, int y, int w, int h, wchar_t glyph, wchar_t col)
{
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			cgeDraw(x + i, y + j, glyph, col);
}

void cgeDrawHorizontalLine(int start, int end, int y, wchar_t glyph, short col)
{
	for (int i = start; i <= end; i++)
		cgeDraw(i, y, glyph, col);
}

void cgeDrawCircle(int x, int y, int r, wchar_t glyph, short col)
{
	if (r <= 0) return;

	int x1 = 0;
	int y1 = r;
	int p = 3 - 2 * r;

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

void cgeFillCircle(int x, int y, int r, wchar_t glyph, short col)
{
	if (r <= 0) return;

	int x1 = 0;
	int y1 = r;
	int p = 3 - 2 * r;

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

	cgeBool changed1 = CGE_FALSE;
	cgeBool changed2 = CGE_FALSE;

	int signx1, signx2, dx1, dy1, dx2, dy2;
	int e1, e2;

	if (y1 > y2) { cgeUtilSwapInt(&y1, &y2); cgeUtilSwapInt(&x1, &x2); }
	if (y1 > y3) { cgeUtilSwapInt(&y1, &y3); cgeUtilSwapInt(&x1, &x3); }
	if (y2 > y3) { cgeUtilSwapInt(&y2, &y3); cgeUtilSwapInt(&x2, &x3); }

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
		cgeUtilSwapInt(&dx1, &dy1);
		changed1 = CGE_TRUE;
	}

	if (dy2 > dx2)
	{
		cgeUtilSwapInt(&dy2, &dx2);
		changed2 = CGE_TRUE;
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
		cgeUtilSwapInt(&dy1, &dx1);
		changed1 = CGE_TRUE;
	}
	else
		changed1 = CGE_FALSE;

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

void cgeDrawRectangle(int x, int y, int w, int h, wchar_t glyph, short col)
{
	for (int i = 0; i <= w; i++)
	{
		cgeDraw(x + i, y, glyph, col);
		cgeDraw(x + i, y + h, glyph, col);
	}

	for (int j = 0; j <= h; j++)
	{
		cgeDraw(x, y + j, glyph, col);
		cgeDraw(x + w, y + j, glyph, col);
	}
}

void cgeDrawSprite(int x, int y, const PSprite const pSprite)
{
	if (!pSprite) return;

	for (int i = 0; i < pSprite->nWidth; i++)
		for (int j = 0; j < pSprite->nHeight; j++)
		{
			short glyph = spriteGetGlyph(pSprite, i, j);
			short col = spriteGetColour(pSprite, i, j);

			cgeDraw(x + i, y + j, glyph, col | col * 16);
		}
}

void cgeDrawSpriteAlpha(int x, int y, const PSprite const pSprite)
{
	if (!pSprite) return;

	for (int i = 0; i < pSprite->nWidth; i++)
		for (int j = 0; j < pSprite->nHeight; j++)
		{
			short glyph = spriteGetGlyph(pSprite, i, j);

			if (glyph != L' ')
			{
				short col = spriteGetColour(pSprite, i, j);
				cgeDraw(x + i, y + j, glyph, col | col * 16);
			}
		}
}

void cgeDrawPartialSprite(int x, int y, int fx, int fy, int fw, int fh, PSprite pSprite)
{
	if (!pSprite) return;

	for (int i = fx, x1 = 0; i < fx + fw; i++, x1++)
		for (int j = fy, y1 = 0; j < fy + fh; j++, y1++)
		{
			short glyph = spriteGetGlyph(pSprite, i, j);
			short col = spriteGetColour(pSprite, i, j);

			cgeDraw(x + x1, y + y1, glyph, col | col * 16);
		}
}

void cgeDrawPartialSpriteAlpha(int x, int y, int fx, int fy, int fw, int fh, PSprite pSprite)
{
	if (!pSprite) return;

	for (int i = fx, x1 = 0; i < fx + fw; i++, x1++)
		for (int j = fy, y1 = 0; j < fy + fh; j++, y1++)
		{
			short glyph = spriteGetGlyph(pSprite, i, j);

			if (glyph != L' ')
			{
				short col = spriteGetColour(pSprite, i, j);
				cgeDraw(x + x1, y + y1, glyph, col | col * 16);
			}
		}
}

void cgeDrawString(int x, int y, const wchar_t* text, short col)
{
	size_t len = lstrlenW(text);

	if (x + len < _engine.nScreenWidth && x >= 0 && y >= 0 && y < _engine.nScreenHeight)
	{
		for (size_t i = 0; i < len; i++)
		{
			_engine.pScreen[y * _engine.nScreenWidth + x + i].Char.UnicodeChar = text[i];
			_engine.pScreen[y * _engine.nScreenWidth + x + i].Attributes = col;
		}
	}
}

void cgeClear(wchar_t glyph, short col)
{
	cgeFillRectangle(0, 0, _engine.nScreenWidth, _engine.nScreenHeight, glyph, col);
}

cgeBool cgeIsFocused()
{
	return _engine.bIsFocused;
}

int cgeGetMouseX()
{
	return _engine.nMouseX;
}

int cgeGetMouseY()
{
	return _engine.nMouseY;
}

KeyState cgeGetMouse(int button)
{
	return _engine.aryMouse[button];
}

KeyState cgeGetKey(int key)
{
	return _engine.aryKeys[key];
}

float cgeGetDeltaTime()
{
	return _engine.fDeltaTime;
}

int cgeScreenWidth()
{
	return _engine.nScreenWidth;
}

int cgeScreenHeight()
{
	return _engine.nScreenHeight;
}