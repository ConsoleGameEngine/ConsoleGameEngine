#define CGE_IMPL
#include "ConsoleGameEngine.hpp"

using namespace std;

constexpr int aGlyphs[4] = { PIXEL_SOLID, PIXEL_QUARTER, PIXEL_HALF, PIXEL_THREEQUARTERS };

class Example : public ConsoleGameEngine
{
public:
	Example() {}

	inline static wstring s_sFileName;
	static int s_nWidth;
	static int s_nHeight;

	Sprite* sprCanvas = nullptr;

	int nGlyph = 0;
	int nColour = FG_WHITE;

protected:
	bool OnUserCreate() override
	{
		if (s_nWidth == 0 || s_nHeight == 0)
		{
			if (s_sFileName.empty())
			{
				// Create a canvas without a size
				sprCanvas = new Sprite();

				// Set a default filename
				s_sFileName = L"untitled.spr";
			}
			else
			{
				// Create the canvas and set load its content from a file
				sprCanvas = new Sprite(s_sFileName);
			}
		}
		else
		{
			// Create the canvas with a size
			sprCanvas = new Sprite(s_nWidth, s_nHeight);
		}

		sAppName = s_sFileName;

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetKey(VK_ESCAPE).bPressed)
			return false;

		if (GetKey(L'S').bPressed)
			sprCanvas->Save(s_sFileName);

		if (GetKey(VK_UP).bPressed) nColour--;
		if (GetKey(VK_DOWN).bPressed) nColour++;

		if (GetKey(VK_LEFT).bPressed) nGlyph--;
		if (GetKey(VK_RIGHT).bPressed) nGlyph++;

		if (nColour < 0x0) nColour = 0xF;
		if (nColour > 0xF) nColour = 0x0;

		if (nGlyph < 0) nGlyph = 3;
		if (nGlyph > 3) nGlyph = 0;

		int nCanvasStartX = ScreenWidth() * 0.3f;
		int nCanvasStartY = ScreenHeight() * 0.3f;

		int nCanvasEndX = nCanvasStartX + sprCanvas->nWidth;
		int nCanvasEndY = nCanvasStartY + sprCanvas->nHeight;

		int nMouseX = MouseX();
		int nMouseY = MouseY();

		int nNormalisedX = nMouseX - nCanvasStartX;
		int nNormalisedY = nMouseY - nCanvasStartY;

		if (nNormalisedX >= 0 && nNormalisedY >= 0 && nNormalisedX < sprCanvas->nWidth && nNormalisedY < sprCanvas->nHeight)
		{
			if (GetMouse(0).bHeld)
			{
				sprCanvas->SetGlyph(nNormalisedX, nNormalisedY, aGlyphs[nGlyph]);
				sprCanvas->SetColour(nNormalisedX, nNormalisedY, nColour);
			}

			if (GetMouse(1).bHeld)
			{
				sprCanvas->SetGlyph(nNormalisedX, nNormalisedY, PIXEL_SOLID);
				sprCanvas->SetColour(nNormalisedX, nNormalisedY, FG_BLACK);
			}
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		// Draw an outline
		DrawLine(nCanvasStartX - 1, nCanvasStartY - 1, nCanvasEndX, nCanvasStartY - 1, PIXEL_SOLID, FG_RED | BG_RED);
		DrawLine(nCanvasStartX - 1, nCanvasEndY, nCanvasEndX, nCanvasEndY, PIXEL_SOLID, FG_RED | BG_RED);
		DrawLine(nCanvasStartX - 1, nCanvasStartY - 1, nCanvasStartX - 1, nCanvasEndY, PIXEL_SOLID, FG_RED | BG_RED);
		DrawLine(nCanvasEndX, nCanvasStartY - 1, nCanvasEndX, nCanvasEndY, PIXEL_SOLID, FG_RED | BG_RED);

		// Draw the canvas
		DrawSprite(nCanvasStartX, nCanvasStartY, sprCanvas);

		int y = 0;

		// Display info about a current state of the program
		DrawString(0, y++, L"Brush: ", FG_WHITE); Draw(8, y - 1, aGlyphs[nGlyph], nColour);
		DrawString(0, y++, L"Mouse: " + to_wstring(nNormalisedX) + L", " + to_wstring(nNormalisedY), FG_WHITE);

		y++;

		// Display controls
		DrawString(0, y++, L"Change brush glyph: ←, →", FG_WHITE);
		DrawString(0, y++, L"Change brush colour: ↑, ↓", FG_WHITE);
		DrawString(0, y++, L"Save: S", FG_WHITE);
		DrawString(0, y++, L"Exit: ESC", FG_WHITE);

		y++;
		
		// Display a glyph table
		DrawString(0, y++, L"Glyphs:");
		DrawString(0, y++, L"PIXEL_SOLID = 9608");
		DrawString(0, y++, L"PIXEL_QUARTER = 9617");
		DrawString(0, y++, L"PIXEL_HALF = 9618");
		DrawString(0, y++, L"PIXEL_THREEQUARTERS = 9619");

		y++;

		// Display a colour table
		DrawString(0, y++, L"Colours:");
		DrawString(0, y++, L"FG_BLACK = 0");
		DrawString(0, y++, L"FG_DARK_BLUE = 1");
		DrawString(0, y++, L"FG_DARK_GREEN = 2");
		DrawString(0, y++, L"FG_DARK_CYAN = 3");
		DrawString(0, y++, L"FG_DARK_RED = 4");
		DrawString(0, y++, L"FG_DARK_MAGENTA = 5");
		DrawString(0, y++, L"FG_DARK_YELLOW = 6");
		DrawString(0, y++, L"FG_GREY = 7");
		DrawString(0, y++, L"FG_DARK_GREY = 8");
		DrawString(0, y++, L"FG_BLUE = 9");
		DrawString(0, y++, L"FG_GREEN = 10");
		DrawString(0, y++, L"FG_CYAN = 11");
		DrawString(0, y++, L"FG_RED = 12");
		DrawString(0, y++, L"FG_MAGENTA = 13");
		DrawString(0, y++, L"FG_YELLOW = 14");
		DrawString(0, y++, L"FG_WHITE = 15");

		return true;
	}
};

int Example::s_nWidth = 0;
int Example::s_nHeight = 0;

int main(int argc, char** argv)
{
	Example demo;

	if (argc >= 2)
	{
		Example::s_sFileName.resize(strlen(argv[1]));
		mbstowcs(Example::s_sFileName.data(), argv[1], Example::s_sFileName.size());

		if (argc == 3)
		{
			int nWidth = atoi(argv[2]);
			Example::s_nWidth = Example::s_nHeight = nWidth;
		}

		if (argc == 4)
		{
			Example::s_nWidth = atoi(argv[2]);
			Example::s_nHeight = atoi(argv[3]);
		}
	}

	if (demo.ConstructConsole(80, 80, 10, 10) == rcode::OK)
		demo.Run();

	return 0;
}
