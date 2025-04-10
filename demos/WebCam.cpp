// ESCAPI: https://github.com/jarikomppa/escapi

#define CONSOLE_GAME_ENGINE_IMPLEMENTATION
#include "ConsoleGameEngine.hpp"

#include "escapi.h"

constexpr int nVariates = 16;

class Example : public ConsoleGameEngine
{
public:
	Example()
	{
		sAppName = L"Example";
		sFont = L"Times New Roman";
	}

	SimpleCapParams capture;
	int nCameras;

private:
	union Color
	{
		int rgb;
		uint8_t c[4];
	};

protected:
	bool OnUserCreate() override
	{
		nCameras = setupESCAPI();

		if (nCameras == 0)
			return false;

		capture.mWidth = ScreenWidth();
		capture.mHeight = ScreenHeight();
		capture.mTargetBuf = new int[capture.mWidth * capture.mHeight];

		if (initCapture(0, &capture) == 0)
			return false;

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		doCapture(0);
		while (isCaptureDone(0) == 0) {}

		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
			{
				// Get a pixel

				Color pixel;
				pixel.rgb = capture.mTargetBuf[y * ScreenWidth() + x];

				// Convert to the Command Prompt pixel analogue

				float fLuminance = (0.2987 * pixel.c[2] + 0.587 * pixel.c[1] + 0.114 * pixel.c[0]) / 255.0 * nVariates;

				wchar_t c = (int)fLuminance + 'A';

				// Display the pixel
				Draw(x, y, c, FG_WHITE);
			}

		return true;
	}
};

int main()
{
	Example demo;

	if (demo.ConstructConsole(256, 240, 4, 4) == RC_OK)
		demo.Run();

	return 0;
}
