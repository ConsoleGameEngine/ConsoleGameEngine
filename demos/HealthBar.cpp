#include "ConsoleGameEngine.h"

class Example : public def::ConsoleGameEngine
{
public:
	Example()
	{
		sAppName = L"ConsoleGameEngine Sample";
	}

private:
	float fHealth = 1.0f;

	int nBarWidth = 100;
	int nBarHeight = 20;

protected:
	virtual bool OnUserCreate() override
	{
		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::FG::BLACK);

		FillRectangleS(10, 10, nBarWidth, nBarHeight, def::PIXEL_SOLID, def::FG_WHITE);
		FillRectangleS(10, 10, nBarWidth * fHealth, nBarHeight, def::PIXEL_SOLID, def::FG_RED);

		if (GetKey(VK_LEFT).bHeld)
			fHealth -= 2.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			fHealth += 2.0f * fDeltaTime;
		
		if (fHealth < 0.0f)
			fHealth = 0.0f;

		if (fHealth > 1.0f)
			fHealth = 1.0f;

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
