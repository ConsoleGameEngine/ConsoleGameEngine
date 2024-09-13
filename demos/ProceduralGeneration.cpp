#define CGE_IMPL
#include "ConsoleGameEngine.hpp"

class Example : public ConsoleGameEngine
{
public:
	Example()
	{
		sAppName = L"Example";
	}

private:
	uint32_t Lehmer(uint32_t seed)
	{
		uint32_t nLehmer;
		uint64_t tmp;

		nLehmer = seed + 0xE120FC15;
		tmp = (uint64_t)nLehmer * 0x4A39B70D;

		uint32_t m1 = (tmp >> 32) ^ tmp;
		tmp = (uint64_t)m1 * 0x12FAD5C9;

		uint32_t m2 = (tmp >> 32) ^ tmp;
		return m2;
	}

	void Wrap(uint32_t& n, uint32_t min, uint32_t max)
	{
		n = (n % (max - min)) + min;
	}

	bool bPanStarted = false;

	int nStartPanX = 0;
	int nStartPanY = 0;

	int nOffsetX = 0;
	int nOffsetY = 0;

protected:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetMouse(0).bPressed)
		{
			nStartPanX = MouseX();
			nStartPanY = MouseY();
			bPanStarted = true;
		}

		if (GetMouse(0).bReleased)
			bPanStarted = false;

		if (bPanStarted)
		{
			int32_t nMouseX = MouseX();
			int32_t nMouseY = MouseY();

			nOffsetX += nStartPanX - nMouseX;
			nOffsetY += nStartPanY - nMouseY;

			nStartPanX = nMouseX;
			nStartPanY = nMouseY;
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		for (int i = 0; i < ScreenWidth(); i++)
			for (int j = 0; j < ScreenHeight(); j++)
			{
				uint32_t nSeed = (j + nOffsetY) << 16 | (i + nOffsetX);

				uint32_t nRand = Lehmer(nSeed);

				bool bIsPlanet = nRand % 2048 < 4;

				uint32_t nRadius = nRand;
				Wrap(nRadius, 3, 8);

				uint32_t nCol = nRand;
				Wrap(nCol, 1, 15);

				if (bIsPlanet)
					FillCircle(i, j, nRadius, PIXEL_SOLID, (int16_t)nCol);
			}

		return true;
	}
};

int main()
{
	Example demo;

	if (demo.ConstructConsole(256, 240, 4, 4) == rcode::OK)
		demo.Run();

	return 0;
}
