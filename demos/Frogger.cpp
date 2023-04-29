#define CGE_APP
#include "ConsoleGameEngine.h"

using namespace std;

class Frogger : public ConsoleGameEngine
{
public:
	Frogger()
	{
		sAppName = L"Frogger";
	}

private:
	Sprite* sprBus = nullptr;
	Sprite* sprCar1 = nullptr;
	Sprite* sprCar2 = nullptr;
	Sprite* sprFrog = nullptr;
	Sprite* sprHome = nullptr;
	Sprite* sprLog = nullptr;
	Sprite* sprPavement = nullptr;
	Sprite* sprWall = nullptr;
	Sprite* sprWater = nullptr;

	vector<pair<float, string>> vecLines = {
		{  0.0f, "oohhooohhooohhoo""oohhooohhooohhoo" },
		{ -3.5f, "wwqstwwwwwqstwww""wwqstwwwwwqstwww" },
		{  3.5f, "wqstwwwqstwwqstw""wqstwwwqstwwqstw" },
		{ -3.5f, "wwwqstwwwqstwwww""wwwqstwwwqstwwww" },
		{  0.0f, "pppppppppppppppp""pppppppppppppppp" },
		{  3.5f, "rabcdrrrrefrrrrr""rabcdrrrrefrrrrr" },
		{ -3.5f, "rrrrrgkrrrrrrrrr""rrrrrgkrrrrrrrrr" },
		{  3.5f, "rrefrrrrrabcdrrr""rrefrrrrrabcdrrr" },
		{  0.0f, "pppppppppppppppp""pppppppppppppppp" }
	};

	int nTileSize = 8;

	float fFrogX = 8.0f;
	float fFrogY = 8.0f;

	float fGlobalTime = 0.0f;

public:
	virtual bool OnUserCreate() override
	{
		sprBus = new Sprite(L"frogger_sprites/bus.spr");
		sprCar1 = new Sprite(L"frogger_sprites/car1.spr");
		sprCar2 = new Sprite(L"frogger_sprites/car2.spr");
		sprFrog = new Sprite(L"frogger_sprites/frog.spr");
		sprHome = new Sprite(L"frogger_sprites/home.spr");
		sprLog = new Sprite(L"frogger_sprites/log.spr");
		sprPavement = new Sprite(L"frogger_sprites/pavement.spr");
		sprWall = new Sprite(L"frogger_sprites/wall.spr");
		sprWater = new Sprite(L"frogger_sprites/water.spr");

		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		fGlobalTime += fDeltaTime;

		float fPotentialX = fFrogX;
		float fPotentialY = fFrogY;

		if (GetKey(VK_LEFT).bPressed)	fPotentialX -= 1.0f;
		if (GetKey(VK_RIGHT).bPressed)	fPotentialX += 1.0f;
		if (GetKey(VK_UP).bPressed)		fPotentialY -= 1.0f;
		if (GetKey(VK_DOWN).bPressed)	fPotentialY += 1.0f;

		int nTileY = (int)fPotentialY;
		int nTileX = 0;

		int nOffset = (int)(vecLines[nTileY].first * fGlobalTime * (float)nTileSize) % (ScreenWidth() * 2);

		for (auto tile : vecLines[nTileY].second)
		{
			int x1 = (int)(fPotentialX * (float)nTileSize) + nTileSize;
			int y1 = (int)(fPotentialY * (float)nTileSize);

			int x2 = (nTileX * nTileSize + nOffset) % (ScreenWidth() * 2);
			int y2 = nTileY * nTileSize;
			if (x2 < 0) x2 = (ScreenWidth() * 2) - (abs(x2) % (ScreenWidth() * 2));

			if (x1 < x2 + nTileSize && y1 < y2 + nTileSize &&
				x2 < x1 + nTileSize && y2 < y1 + nTileSize)
			{
				if (string("o").find(tile) != string::npos)
				{
					fPotentialX = fFrogX;
					fPotentialY = fFrogY;
				}
				else if (string("abcdfgkhw").find(tile) != string::npos)
				{
					fPotentialX = 8.0f;
					fPotentialY = 8.0f;
				}
				else if (string("qst").find(tile) != string::npos)
				{
					fPotentialX += vecLines[nTileY].first * fDeltaTime;
				}
			}

			nTileX++;
		}

		fFrogX = fPotentialX;
		fFrogY = fPotentialY;

		Clear(PIXEL_SOLID, FG_BLACK);

		nTileY = 0;
		for (auto& line : vecLines)
		{
			int nOffset = (int)(line.first * fGlobalTime * (float)nTileSize) % (ScreenWidth() * 2);

			nTileX = 0;
			for (const auto& tile : line.second)
			{
				int x = (nTileX * nTileSize + nOffset) % (ScreenWidth() * 2);
				int y = nTileY * nTileSize;
				if (x < 0) x = (ScreenWidth() * 2) - (abs(x) % (ScreenWidth() * 2));
				x -= nTileSize;

				switch (tile)
				{
				case 'r': FillRectangle(x, y, nTileSize, nTileSize, PIXEL_SOLID, FG_BLACK); break;
				case 'h': DrawSprite(x, y, sprHome);		break;
				case 'p': DrawSprite(x, y, sprPavement);	break;
				case 'o': DrawSprite(x, y, sprWall);		break;
				case 'w': DrawSprite(x, y, sprWater);		break;

				case 'a': DrawPartialSprite(x, y, 0, 0, 8, 8, sprBus);	break;
				case 'b': DrawPartialSprite(x, y, 8, 0, 8, 8, sprBus);	break;
				case 'c': DrawPartialSprite(x, y, 16, 0, 8, 8, sprBus); break;
				case 'd': DrawPartialSprite(x, y, 24, 0, 8, 8, sprBus); break;

				case 'e': DrawPartialSprite(x, y, 0, 0, 8, 8, sprCar1); break;
				case 'f': DrawPartialSprite(x, y, 8, 0, 8, 8, sprCar1); break;

				case 'g': DrawPartialSprite(x, y, 0, 0, 8, 8, sprCar2); break;
				case 'k': DrawPartialSprite(x, y, 8, 0, 8, 8, sprCar2); break;

				case 'q': DrawPartialSprite(x, y, 0, 0, 8, 8, sprLog);	break;
				case 's': DrawPartialSprite(x, y, 8, 0, 8, 8, sprLog);	break;
				case 't': DrawPartialSprite(x, y, 16, 0, 8, 8, sprLog); break;
				}

				nTileX++;
			}

			nTileY++;
		}

		DrawSprite(fFrogX * (float)nTileSize, fFrogY * (float)nTileSize, sprFrog);

		return true;
	}

};

int main()
{
	Frogger app;

	if (app.ConstructConsole(128, 72, 8, 8) == RCODE_OK)
		app.Run();

	return 0;
}
