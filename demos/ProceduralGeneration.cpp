#include "ConsolaProd.h"

using namespace def;

class Example : public def::ConsolaProd
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

	void Clamp(uint32_t& n, uint32_t min, uint32_t max)
	{
		n = (n % (max - min)) + min;
	}

	bool bPanStarted = false;

	def::vi2d vStartPan = { 0, 0 };
	def::vi2d vOffset = { 0, 0 };

protected:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{

		if (GetMouse(0).bPressed)
		{
			vStartPan.x = GetMouseX();
			vStartPan.y = GetMouseY();
			bPanStarted = true;
		}

		if (GetMouse(0).bReleased)
			bPanStarted = false;

		if (bPanStarted)
		{
			int32_t nMouseX = GetMouseX();
			int32_t nMouseY = GetMouseY();

			vOffset.x += vStartPan.x - nMouseX;
			vOffset.y += vStartPan.y - nMouseY;

			if (vOffset.x < 0)
				vOffset.x = 0;

			vStartPan.x = nMouseX;
			vStartPan.y = nMouseY;
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		for (int i = 0; i < GetScreenWidth(); i++)
			for (int j = 0; j < GetScreenHeight(); j++)
			{
				uint32_t nSeed = (j + vOffset.y) << 16 | (i + vOffset.x);

				uint32_t nRand = Lehmer(nSeed);

				bool bIsPlanet = nRand % 2048 < 4;

				uint32_t nRadius = nRand;
				Clamp(nRadius, 3, 8);

				uint32_t nCol = nRand;
				Clamp(nCol, 1, 15);

				if (bIsPlanet)
					FillCircle(i, j, nRadius, PIXEL_SOLID, (int16_t)nCol);
			}

		return true;
	}
};

int main()
{
	Example demo;
	def::rcode err = demo.ConstructConsole(256, 240, 4, 4);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
