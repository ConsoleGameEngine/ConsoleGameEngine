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

	bool bPanStarted = false;

	def::vi2d vStartPan = { 0, 0 };
	def::vi2d vOffset = { 0, 0 };

	void DrawStars()
	{
		for (int i = 0; i < GetScreenWidth(); i++)
			for (int j = 0; j < GetScreenHeight(); j++)
			{
				uint32_t nSeed = (j + vOffset.y) << 16 | (i + vOffset.x);

				bool bIsWhite = Lehmer(nSeed) % 256 < 32;

				Draw(i, j, PIXEL_SOLID, bIsWhite ? FG_WHITE : FG_BLACK);
			}
	}

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

		if (bPanStarted)
		{
			float fMouseX = GetMouseX();
			float fMouseY = GetMouseY();

			vOffset.x += vStartPan.x - fMouseX;
			vOffset.y += vStartPan.y - fMouseY;

			if (vOffset.x < 0)
				vOffset.x = 0;

			vStartPan.x = fMouseX;
			vStartPan.y = fMouseY;

			DrawStars();
		}

		if (GetMouse(0).bReleased)
			bPanStarted = false;

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
