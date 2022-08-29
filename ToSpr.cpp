#include <iostream>
#include <cstdint>
#include <algorithm>

// Disabling warning about fopen function
#pragma warning(disable : 4996)

// Undefined macros to use std::min and std::max functions
#undef min
#undef max

// Including stb_image library
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Some usefull constants from Consola Prod game engine header file

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

// Function that converts RGB value to colours and glyphs that Consola Prod game engine uses
void ClassifyPixel_Cmd(float r, float g, float b, wchar_t& sym, short& fg_col, short& bg_col)
{
	// Is pixel coloured (i.e. RGB values exhibit significant variance)
	float fMean = (r + g + b) / 3.0f;
	float fRVar = (r - fMean) * (r - fMean);
	float fGVar = (g - fMean) * (g - fMean);
	float fBVar = (b - fMean) * (b - fMean);

	// Pixel has colour so get dostd::minant colour
	float y = std::min(r, g);// (r * g);
	float c = std::min(g, b);// (g * b);
	float m = std::min(b, r);// (b * r);

	float fMean2 = (y + c + m) / 3.0f;
	float fYVar = (y - fMean2) * (y - fMean2);
	float fCVar = (c - fMean2) * (c - fMean2);
	float fMVar = (m - fMean2) * (m - fMean2);

	float fMaxPrimaryVar = std::max(fRVar, fGVar);
	fMaxPrimaryVar = std::max(fMaxPrimaryVar, fBVar);

	float fMaxSecondaryVar = std::max(fCVar, fYVar);
	fMaxSecondaryVar = std::max(fMaxSecondaryVar, fMVar);

	float fShading = 0.5f;

	auto compare = [&](float fV1, float fV2, float fC1, float fC2, short FG_LIGHT, short FG_DARK, short BG_LIGHT, short BG_DARK)
	{
		if (fV1 >= fV2)
		{
			fg_col = fC1 > 0.5f ? FG_LIGHT : FG_DARK;

			if (fV2 < 0.0001f)
			{
				// Secondary is not variant, Use greyscale in background
				if (fC2 >= 0.00f && fC2 < 0.25f) bg_col = BG_BLACK;
				if (fC2 >= 0.25f && fC2 < 0.50f) bg_col = BG_DARK_GREY;
				if (fC2 >= 0.50f && fC2 < 0.75f) bg_col = BG_GREY;
				if (fC2 >= 0.75f && fC2 <= 1.0f) bg_col = BG_WHITE;
			}
			else
			{
				// Secondary is varient, use in background
				bg_col = fC2 > 0.5f ? BG_LIGHT : BG_DARK;
			}

			fShading = ((fC1 - fC2) / 2.0f) + 0.5f;
		}

		if (fShading >= 0.00f && fShading < 0.20f) sym = L' ';
		if (fShading >= 0.20f && fShading < 0.40f) sym = PIXEL_QUARTER;
		if (fShading >= 0.40f && fShading < 0.60f) sym = PIXEL_HALF;
		if (fShading >= 0.60f && fShading < 0.80f) sym = PIXEL_THREEQUARTERS;
		if (fShading >= 0.80f) sym = PIXEL_SOLID;
	};

	if (fRVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
		compare(fRVar, fYVar, r, y, FG_RED, FG_DARK_RED, BG_YELLOW, BG_DARK_YELLOW);

	if (fRVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
		compare(fRVar, fMVar, r, m, FG_RED, FG_DARK_RED, BG_MAGENTA, BG_DARK_MAGENTA);

	if (fRVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
		compare(fRVar, fCVar, r, c, FG_RED, FG_DARK_RED, BG_CYAN, BG_DARK_CYAN);

	if (fGVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
		compare(fGVar, fYVar, g, y, FG_GREEN, FG_DARK_GREEN, BG_YELLOW, BG_DARK_YELLOW);

	if (fGVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
		compare(fGVar, fCVar, g, c, FG_GREEN, FG_DARK_GREEN, BG_CYAN, BG_DARK_CYAN);

	if (fGVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
		compare(fGVar, fMVar, g, m, FG_GREEN, FG_DARK_GREEN, BG_MAGENTA, BG_DARK_MAGENTA);

	if (fBVar == fMaxPrimaryVar && fMVar == fMaxSecondaryVar)
		compare(fBVar, fMVar, b, m, FG_BLUE, FG_DARK_BLUE, BG_MAGENTA, BG_DARK_MAGENTA);

	if (fBVar == fMaxPrimaryVar && fCVar == fMaxSecondaryVar)
		compare(fBVar, fCVar, b, c, FG_BLUE, FG_DARK_BLUE, BG_CYAN, BG_DARK_CYAN);

	if (fBVar == fMaxPrimaryVar && fYVar == fMaxSecondaryVar)
		compare(fBVar, fYVar, b, y, FG_BLUE, FG_DARK_BLUE, BG_YELLOW, BG_DARK_YELLOW);
}

int main(int argc, char* argv[])
{
	// User should provide name of file with image that should be converted to .spr file
	if (argc < 2)
	{
		std::cerr << "Please provide filename!\n";
		return 1;
	}

	std::string sFilename = argv[1];

	uint8_t* pBuffer;
	int nWidth, nHeight, nFormat;

	// Open image file to read
	FILE* f;
	f = fopen(sFilename.c_str(), "rb");

	// If could not open file
	if (!f)
	{
		std::cerr << "Could not open " << sFilename << '\n';
		return 1;
	}

	// Load pixels to the buffer using stb_image library
	pBuffer = stbi_load_from_file(f, &nWidth, &nHeight, &nFormat, 0);

	// Close file and free memory
	fclose(f);

	// If could not load pixels in buffer
	if (!pBuffer)
	{
		std::cerr << stbi_failure_reason() << '\n';
		return 1;
	}

	// Check if it's RGB or RGBA image
	if (nFormat != 3 && nFormat != 4)
	{
		std::cerr << "Unsupported color format\n";
		return 1;
	}

	// Buffers for glyphs and colours
	short* m_nGlyphs = new short[nWidth * nHeight];
	short* m_nColours = new short[nWidth * nHeight];

	// Converting RGB or RGBA pixels to fill glyphs and colours buffer
	for (int i = 0; i < nWidth; i++)
		for (int j = 0; j < nHeight; j++)
		{
			// Getting pixels from buffer
			uint8_t* offset = pBuffer + (j * nWidth + i) * nFormat;

			// Getting alpha value of pixel and convert it to floating point number (0 - 1)
			float a = offset[3] / 255.0f;

			// If it has transparency then we mark it as blank cell
			// else we convert RGB pixel to glyph and colour values
			if (a < 1.0f)
			{
				m_nColours[j * nWidth + i] = FG_BLACK;
				m_nGlyphs[j * nWidth + i] = L' ';
			}
			else
			{
				// Now converting RGB values to floating point numbers (0 - 1)
				float r = offset[0] / 255.0f;
				float g = offset[1] / 255.0f;
				float b = offset[2] / 255.0f;
				
				wchar_t sym;
				short fg_col;
				short bg_col;

				ClassifyPixel_Cmd(r, g, b, sym, fg_col, bg_col);

				m_nColours[j * nWidth + i] = fg_col | bg_col;
				m_nGlyphs[j * nWidth + i] = sym;
			}
		}

	// Extracting filename without format...
	sFilename.erase(sFilename.begin() + sFilename.find_last_of('.'), sFilename.end());

	// ...and setting name of new output file
	std::wstring name = std::wstring(sFilename.begin(), sFilename.end()) + L".spr";

	// Open a file to write data
	_wfopen_s(&f, name.c_str(), L"wb");

	// If we can't create a file, then probably we don't have permission to do that
	if (!f)
	{
		std::cerr << "Has no permission to create a new file!\n";
		return 1;
	}

	// Constructing .spr file

	// Writing width and height of sprite
	fwrite(&nWidth, sizeof(int), 1, f);
	fwrite(&nHeight, sizeof(int), 1, f);

	// Writing colours and glyphs of sprite
	fwrite(m_nColours, sizeof(short), nWidth * nHeight, f);
	fwrite(m_nGlyphs, sizeof(short), nWidth * nHeight, f);

	// Close file and free memory
	fclose(f);

	return 0;
}
