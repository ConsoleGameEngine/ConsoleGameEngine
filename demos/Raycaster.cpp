#define CONSOLE_GAME_ENGINE_IMPLEMENTATION
#include "ConsoleGameEngine.hpp"

#undef min
#undef max

#include <algorithm>
#include <list>

using namespace std;

class FirstPersonShooter : public ConsoleGameEngine
{
public:
	FirstPersonShooter()
	{
		sAppName = L"FPS in Command Prompt";
	}

	int nMapWidth = 24;
	int nMapHeight = 20;
	wstring sMap;

	float fPlayerX = 22.0f;
	float fPlayerY = 12.0f;
	
	float fPlayerDirX = -1.0f;
	float fPlayerDirY = 0.0f;

	float fPlaneX = 0.0f;
	float fPlaneY = 0.66f;

	float fSpeed = 5.0f;
	float fRotation = 3.0f;

	float fDepth = 16.0f;

protected:
	bool OnUserCreate() override
	{
		sMap += L"#################....###";
		sMap += L"#......................#";
		sMap += L"#..#####...............#";
		sMap += L"#.........#............#";
		sMap += L"..........#............#";
		sMap += L"...##.....#......####..#";
		sMap += L"..........#............#";
		sMap += L"..........#.....#......#";
		sMap += L".......................#";
		sMap += L"....###................#";
		sMap += L"#...#........####......#";
		sMap += L"#...#...#....####.......";
		sMap += L"#...#...................";
		sMap += L"#...#.........###.......";
		sMap += L"#.......................";
		sMap += L"#.....######...........#";
		sMap += L"#..........##..........#";
		sMap += L"#................###...#";
		sMap += L"#......................#";
		sMap += L"###...##################";

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetKey(VK_UP).bHeld)
		{
			float fNewPosX = fPlayerX + fPlayerDirX * fSpeed * fDeltaTime;
			float fNewPosY = fPlayerY + fPlayerDirY * fSpeed * fDeltaTime;

			if ((int)fNewPosX < nMapWidth && (int)fNewPosX >= 0)
			{
				if (sMap[(int)fPlayerY * nMapWidth + (int)fNewPosX] != '#')
					fPlayerX = fNewPosX;
			}

			if ((int)fNewPosY < nMapHeight && (int)fNewPosY >= 0)
			{
				if (sMap[(int)fNewPosY * nMapWidth + (int)fPlayerX] != '#')
					fPlayerY = fNewPosY;
			}
		}

		if (GetKey(VK_DOWN).bHeld)
		{
			float fNewPosX = fPlayerX - fPlayerDirX * fSpeed * fDeltaTime;
			float fNewPosY = fPlayerY - fPlayerDirY * fSpeed * fDeltaTime;

			if ((int)fNewPosX < nMapWidth && (int)fNewPosX >= 0)
			{
				if (sMap[(int)fPlayerY * nMapWidth + (int)fNewPosX] != '#')
					fPlayerX = fNewPosX;
			}

			if ((int)fNewPosY < nMapHeight && (int)fNewPosY >= 0)
			{
				if (sMap[(int)fNewPosY * nMapWidth + (int)fPlayerX] != '#')
					fPlayerY = fNewPosY;
			}
		}

		if (GetKey(VK_RIGHT).bHeld)
		{
			float fOldDirX = fPlayerDirX;
			float fOldPlaneX = fPlaneX;

			fPlayerDirX = fPlayerDirX * cos(-fRotation * fDeltaTime) - fPlayerDirY * sin(-fRotation * fDeltaTime);
			fPlayerDirY = fOldDirX * sin(-fRotation * fDeltaTime) + fPlayerDirY * cos(-fRotation * fDeltaTime);

			fPlaneX = fPlaneX * cos(-fRotation * fDeltaTime) - fPlaneY * sin(-fRotation * fDeltaTime);
			fPlaneY = fOldPlaneX * sin(-fRotation * fDeltaTime) + fPlaneY * cos(-fRotation * fDeltaTime);
		}

		if (GetKey(VK_LEFT).bHeld)
		{
			float fOldDirX = fPlayerDirX;
			float fOldPlaneX = fPlaneX;

			fPlayerDirX = fPlayerDirX * cos(fRotation * fDeltaTime) - fPlayerDirY * sin(fRotation * fDeltaTime);
			fPlayerDirY = fOldDirX * sin(fRotation * fDeltaTime) + fPlayerDirY * cos(fRotation * fDeltaTime);

			fPlaneX = fPlaneX * cos(fRotation * fDeltaTime) - fPlaneY * sin(fRotation * fDeltaTime);
			fPlaneY = fOldPlaneX * sin(fRotation * fDeltaTime) + fPlaneY * cos(fRotation * fDeltaTime);
		}

		list<pair<pair<int, int>, float>> listStripes;

		for (int x = 0; x < ScreenWidth(); x++)
		{
			float fCamera = 2.0f * (float)x / (float)ScreenWidth() - 1.0f;

			float fRayX = fPlayerDirX + fPlaneX * fCamera;
			float fRayY = fPlayerDirY + fPlaneY * fCamera;

			float fCellDistX = (fRayX == 0.0f) ? INFINITY : abs(1.0f / fRayX);
			float fCellDistY = (fRayY == 0.0f) ? INFINITY : abs(1.0f / fRayY);

			int nMapX = (float)fPlayerX;
			int nMapY = (float)fPlayerY;

			float fDistToNextX, fDistToNextY;
			int nStepX, nStepY;

			if (fRayX < 0.0f)
			{
				nStepX = -1;
				fDistToNextX = (fPlayerX - (float)nMapX) * fCellDistX;
			}
			else
			{
				nStepX = 1;
				fDistToNextX = ((float)nMapX - fPlayerX + 1.0f) * fCellDistX;
			}

			if (fRayY < 0.0f)
			{
				nStepY = -1;
				fDistToNextY = (fPlayerY - (float)nMapY) * fCellDistY;
			}
			else
			{
				nStepY = 1;
				fDistToNextY = ((float)nMapY - fPlayerY + 1.0f) * fCellDistY;
			}

			float fDistToWall = 0.0f;

			int nSide;
			bool bHitWall = false;

			while (!bHitWall)
			{
				if (fDistToNextX < fDistToNextY)
				{
					fDistToNextX += fCellDistX;
					nMapX += nStepX;
					nSide = 0;
				}
				else
				{
					fDistToNextY += fCellDistY;
					nMapY += nStepY;
					nSide = 1;
				}

				if (nMapX < 0 || nMapY < 0 || nMapX >= nMapWidth || nMapY >= nMapHeight)
				{
					fDistToWall = fDepth;
					bHitWall = true;
				}
				else
				{
					if (sMap[nMapY * nMapWidth + nMapX] == L'#')
						bHitWall = true;
				}
			}

			if (fDistToWall != fDepth)
			{
				if (nSide == 0)
					fDistToWall = fDistToNextX - fCellDistX;
				else
					fDistToWall = fDistToNextY - fCellDistY;
			}

			float fStripeHeight = (float)ScreenHeight() / fDistToWall;

			int nCeiling = (int)max(-fStripeHeight * 0.5f + (float)ScreenHeight() * 0.5f, 0.0f);
			int nFloor = (int)min(fStripeHeight * 0.5f + (float)ScreenHeight() * 0.5f, (float)ScreenHeight() - 1.0f);

			listStripes.push_back({ { nCeiling, nFloor }, fDistToWall });
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		int x = 0;
		for (const auto& s : listStripes)
		{
			int nCeiling = s.first.first;
			int nFloor = s.first.second;

			for (int y = 0; y <= nFloor; y++)
			{
				if (y <= nCeiling)
				{
					// Ceiling
					Draw(x, y, PIXEL_SOLID, FG_BLACK);

					// Floor
					Draw(x, ScreenHeight() - y, PIXEL_SOLID, FG_GREY);
				}
				else if (s.second < fDepth)
				{
					short nPixel;

					if (s.second / fDepth <= 0.25f) nPixel = PIXEL_SOLID;
					else if (s.second / fDepth <= 0.5f) nPixel = PIXEL_THREEQUARTERS;
					else if (s.second / fDepth <= 0.75f) nPixel = PIXEL_HALF;
					else if (s.second / fDepth <= 1.0f) nPixel = PIXEL_QUARTER;

					// Wall
					Draw(x, y, nPixel, FG_WHITE);
				}
				else
				{
					// Void
					Draw(x, y, PIXEL_SOLID, FG_BLACK);
				}
			}

			x++;
		}

		return true;
	}

};

int main()
{
	FirstPersonShooter demo;
	if (demo.ConstructConsole(256, 240, 4, 4) == RC_OK)
		demo.Run();
	return 0;
}
