#include "ConsolaProd.h"

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Consola Prod Sample";
	}

private:
	def::vf2d vFirst = { 10.0f, 100.0f };
	def::vf2d vSecond = { 150.0f, 30.0f };

	def::vf2d vFactor = { 1.0f, 1.0f };

protected:
	virtual bool OnUserCreate() override
	{
		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::FG::BLACK);

		def::vf2d vQuadPrev = vFirst;
		
		for (float t = 0.0f; t < 1.0f; t += 0.01f)
		{
			def::vf2d vLinear =
			{
				(1 - t) * vFirst.x + t * vSecond.x,
				(1 - t) * vFirst.y + t * vSecond.y
			};

			def::vf2d vQuad =
			{
				vFactor.x * (1 - t) * vLinear.x + t * vLinear.x,
				vFactor.y * (1 - t) * vLinear.y + t * vLinear.y
			};

			if (vQuadPrev.x != vQuad.x && vQuadPrev.y != vQuad.y && vFirst.x != vLinear.x && vFirst.y != vLinear.y)
				DrawLine(vQuadPrev.x, vQuadPrev.y, vQuad.x, vQuad.y, def::Pixel::SOLID, def::FG::WHITE);

			vQuadPrev = vQuad;
		}

		if (GetKey(VK_LEFT).bHeld)
			vFirst.x -= 30.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			vFirst.x += 30.0f * fDeltaTime;

		if (GetKey(VK_UP).bHeld)
			vFirst.y -= 30.0f * fDeltaTime;

		if (GetKey(VK_DOWN).bHeld)
			vFirst.y += 30.0f * fDeltaTime;

		if (GetKey(L'A').bHeld)
			vSecond.x -= 30.0f * fDeltaTime;

		if (GetKey(L'D').bHeld)
			vSecond.x += 30.0f * fDeltaTime;

		if (GetKey(L'W').bHeld)
			vSecond.y -= 30.0f * fDeltaTime;

		if (GetKey(L'S').bHeld)
			vSecond.y += 30.0f * fDeltaTime;

		if (GetKey(L'R').bHeld)
			vFactor.x += fDeltaTime;

		if (GetKey(L'T').bHeld)
			vFactor.x -= fDeltaTime;

		if (GetKey(L'F').bHeld)
			vFactor.y += fDeltaTime;

		if (GetKey(L'G').bHeld)
			vFactor.y -= fDeltaTime;

		return true;
	}
};


int main()
{
	Example demo;
	demo.Run(256, 240, 4, 4);
	return 0;
}
