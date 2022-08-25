#include "ConsolaProd.h"
#include "escapi.h"

using namespace def;

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Example";
	}

protected:
	bool OnUserCreate() override
	{
		nCameras = setupESCAPI();

		if (nCameras == 0)
			return false;

		scpCapture.mWidth = GetScreenWidth();
		scpCapture.mHeight = GetScreenHeight();

		scpCapture.mTargetBuf = new int[GetScreenWidth() * GetScreenHeight()];

		if (!initCapture(0, &scpCapture))
			return false;

		return true;
	}

	void ClassifyPixel_Cmd(float r, float g, float b, wchar_t& sym, short& fg_col, short& bg_col)
	{
		// Is pixel coloured (i.e. RGB values exhibit significant variance)
		float fMean = (r + g + b) / 3.0f;
		float fRVar = (r - fMean) * (r - fMean);
		float fGVar = (g - fMean) * (g - fMean);
		float fBVar = (b - fMean) * (b - fMean);

		// Pixel has colour so get dominant colour
		float y = min(r, g);// (r * g);
		float c = min(g, b);// (g * b);
		float m = min(b, r);// (b * r);

		float fMean2 = (y + c + m) / 3.0f;
		float fYVar = (y - fMean2) * (y - fMean2);
		float fCVar = (c - fMean2) * (c - fMean2);
		float fMVar = (m - fMean2) * (m - fMean2);

		float fMaxPrimaryVar = max(fRVar, fGVar);
		fMaxPrimaryVar = max(fMaxPrimaryVar, fBVar);

		float fMaxSecondaryVar = max(fCVar, fYVar);
		fMaxSecondaryVar = max(fMaxSecondaryVar, fMVar);

		float fShading = 0.5f;

		auto compare = [&](float fV1, float fV2, float fC1, float fC2, short FG_LIGHT, short FG_DARK, short BG_LIGHT, short BG_DARK)
		{
			if (fV1 >= fV2)
			{
				// Primary Is Dominant, Use in foreground
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
					// Secondary is varient, Use in background
					bg_col = fC2 > 0.5f ? BG_LIGHT : BG_DARK;
				}

				// Shade dominant over background (100% -> 0%)
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

	void ClassifyPixel_Grey(float r, float g, float b, wchar_t& sym, short& fg_col, short& bg_col)
	{
		float luminance = 0.2987f * r + 0.5870f * g + 0.1140f * b;
		int pixel_bw = (int)(luminance * 13.0f);
		switch (pixel_bw)
		{
		case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

		case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
		case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
		case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

		case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
		case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
		case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
		case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

		case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
		case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
		case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
		case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
		}
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		doCapture(0);

		for (int x = 0; x < scpCapture.mWidth; x++)
			for (int y = 0; y < scpCapture.mHeight; y++)
			{
				union RGBAint
				{
					int rgb;
					uint8_t c[4];
				};

				RGBAint col;
				col.rgb = scpCapture.mTargetBuf[y * scpCapture.mWidth + x];

				wchar_t sym;
				short bg_col;
				short fg_col;

				float r = col.c[2] / 255.0f;
				float g = col.c[1] / 255.0f;
				float b = col.c[0] / 255.0f;

				ClassifyPixel_Cmd(r, g, b, sym, fg_col, bg_col);
				//ClassifyPixel_Grey(r, g, b, sym, fg_col, bg_col);

				Draw(x, y, sym, fg_col | bg_col);
			}

		return true;
	}

private:
	int nCameras = 0;
	SimpleCapParams scpCapture;
};

int main()
{
	Example demo;
	rcode err = demo.ConstructConsole(256, 240, 4, 4);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
