#define CGE_IMPL
#include "ConsoleGameEngine.hpp"

#include <list>

using namespace std;

class BreakOut : public ConsoleGameEngine
{
public:
	BreakOut()
	{
		sAppName = L"Break Out";
	}

	list<pair<int, int>> listTiles;

	pair<float, int> pairBat;
	int nBatLength;

	pair<float, float> pairBallPos;
	pair<float, float> pairBallVel;

	const float fBatSpeed = 50.0f;
	const float fBallSpeed = 20.0f;
	
	uint32_t nScore = 0;

protected:
	bool OnUserCreate() override
	{
		int nTilesPosX = int((float)ScreenWidth() * 0.34f);
		int nTilesPosY = int((float)ScreenHeight() * 0.2f);

		int nTilesCountX = int((float)ScreenWidth() * 0.3f);
		int nTilesCountY = int((float)ScreenHeight() * 0.1f);

		for (int x = 0; x < nTilesCountX; x++)
			for (int y = 0; y < nTilesCountY; y++)
				listTiles.push_back({ nTilesPosX + x, nTilesPosY + y });

		pairBat.first = (float)ScreenWidth() * 0.45f;
		pairBat.second = int((float)ScreenHeight() * 0.8f);

		nBatLength = int((float)ScreenWidth() * 0.1f);

		pairBallPos.first = (float)ScreenWidth() * 0.5f;
		pairBallPos.second = (float)ScreenHeight() * 0.6f;

		pairBallVel = { -0.5f, 1.0f };

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetKey(VK_RIGHT).bHeld)
			pairBat.first += fDeltaTime * fBatSpeed;

		if (GetKey(VK_LEFT).bHeld)
			pairBat.first -= fDeltaTime * fBatSpeed;

		float fNewVelX = pairBallVel.first;
		float fNewVelY = pairBallVel.second;

		int nNewPosX = int(pairBallPos.first + fNewVelX * fDeltaTime * fBallSpeed);
		int nNewPosY = int(pairBallPos.second + fNewVelY * fDeltaTime * fBallSpeed);

		if (nNewPosX < 0 || nNewPosX >= ScreenWidth())
			fNewVelX *= -1.0f;
		else if (nNewPosY < 0 || nNewPosY >= ScreenHeight())
			fNewVelY *= -1.0f;
		else
		{
			auto itTile = listTiles.begin();

			list<decltype(itTile)> listRedundant;

			for (const auto& [x, y] : listTiles)
			{
				bool bRedundant = false;

				if (nNewPosX == x && (int)pairBallPos.second == y)
				{
					fNewVelX *= -1.0f;
					bRedundant = true;
				}

				if (nNewPosY == y && (int)pairBallPos.first == x)
				{
					fNewVelY *= -1.0f;
					bRedundant = true;
				}

				if (bRedundant)
					listRedundant.push_back(itTile);

				itTile++;
			}

			for (const auto& it : listRedundant)
				listTiles.erase(it);

			nScore += listRedundant.size();
		}

		if (nNewPosY == pairBat.second &&
			roundf(pairBat.first) <= nNewPosX && nNewPosX <= roundf(pairBat.first) + nBatLength)
		{
			fNewVelY *= -1.0f;
		}

		pairBallVel = { fNewVelX, fNewVelY };

		pairBallPos.first += fNewVelX * fDeltaTime * fBallSpeed;
		pairBallPos.second += fNewVelY * fDeltaTime * fBallSpeed;

		Clear(PIXEL_SOLID, FG_BLACK);

		DrawString(1, 1, L"Score: " + to_wstring(nScore), FG_WHITE);

		for (const auto& [x, y] : listTiles)
			Draw(x, y, L'#', FG_WHITE);

		DrawLine((int)pairBat.first, pairBat.second, (int)pairBat.first + nBatLength, pairBat.second, PIXEL_SOLID, FG_RED);

		Draw((int)pairBallPos.first, (int)pairBallPos.second, PIXEL_SOLID, FG_GREEN);

		return true;
	}

};

int main()
{
	BreakOut demo;
	if (demo.ConstructConsole(40, 30, 24, 24) == rcode::OK)
		demo.Run();
	return 0;
}
