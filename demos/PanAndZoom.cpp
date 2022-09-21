#include "ConsoleGameEngine.h"

class PanAndZoom : public def::ConsoleGameEngine
{
public:
	PanAndZoom()
	{
		sAppName = L"Pan And Zoom";
	}

private:
	float fStartPanX = 0.0f;
	float fStartPanY = 0.0f;

	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;

	float fScale = 1.0f;

	bool bPanStarted = false;

protected:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		auto to_screen = [&](float x, float y)
		{
			return def::vi2d(x + fOffsetX, y + fOffsetY);
		};

		Clear(def::PIXEL_SOLID, def::FG_BLACK);

		def::KeyState sLeftState = GetMouseBtn(0);

		if (GetKey(L'Z').bHeld)
			fScale += fDeltaTime;

		if (GetKey(L'X').bHeld)
			fScale -= fDeltaTime;

		if (fScale > 4.0f)
			fScale = 4.0f;

		if (fScale < 0.1f)
			fScale = 0.1f;

		if (sLeftState.bPressed)
		{
			fStartPanX = GetMouseX();
			fStartPanY = GetMouseY();
			bPanStarted = true;
		}

		if (bPanStarted)
		{
			float fMouseX = GetMouseX();
			float fMouseY = GetMouseY();

			fOffsetX += fStartPanX - fMouseX;
			fOffsetY += fStartPanY - fMouseY;

			fStartPanX = fMouseX;
			fStartPanY = fMouseY;
		}

		if (sLeftState.bReleased)
			bPanStarted = false;

		DrawCircle(to_screen(GetScreenWidth() / 2, GetScreenHeight() / 2), 40 * fScale, def::PIXEL_SOLID, def::FG_BLUE);

		return true;
	}
};

int main()
{
	PanAndZoom demo;
	def::rcode err = demo.ConstructConsole(256, 240, 4, 4);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
