#define DEF_CP_APP
#include "ConsolaProd.h"

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Example";
	}

protected:
	virtual bool OnUserCreate() override
	{
		float fAngle = 0.0;
			
		for (int i = 0; i < nScreenWidth; i++)
		{
			int CurrentY[3];

			CurrentY[0] = int(nAmplitude * sinf(fAngle)) + nScreenHeight / 2;
			DrawLine(i - 1, PrevY[0], i, CurrentY[0], def::Pixel::SOLID, def::FG::RED);
			PrevY[0] = CurrentY[0];
			
			CurrentY[1] = int(nAmplitude * cosf(fAngle)) + nScreenHeight / 2;
			DrawLine(i - 1, PrevY[1], i, CurrentY[1], def::Pixel::SOLID, def::FG::YELLOW);
			PrevY[1] = CurrentY[1];

			CurrentY[2] = int(nAmplitude * tanf(fAngle)) + nScreenHeight / 2;
			DrawLine(i - 1, PrevY[2], i, CurrentY[2], def::Pixel::SOLID, def::FG::GREEN);
			PrevY[2] = CurrentY[2];

			fAngle += (2.0f * 3.1415926535f) / 50;
		}

		return true;
	}

	virtual bool OnUserUpdate(float dt) override
	{
		return true;
	}

private:
	const int nAmplitude = 10;
	int PrevY[3] = { 0, 0, 0 };

};

int main()
{
	Example demo;
	demo.Run(256, 240, 4, 4);
}
