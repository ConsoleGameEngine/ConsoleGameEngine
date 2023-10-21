#define CGE_IMPL
#include "ConsoleGameEngine.hpp"

#include <array>

constexpr int nScreenWidth = 40;
constexpr int nScreenHeight = 30;

constexpr int nMines = 200;

struct SCell
{
	bool bMine = false;
	bool bOpen = false;
	bool bFlag = false;
	int nAround = 0;
};

struct SWorld
{
	std::array<SCell, nScreenWidth * nScreenHeight> aryMap;

	int nTotalMines = 0;
	int nClosedCells = 0;
	bool bFailed = false;

	bool Inside(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < nScreenWidth && y < nScreenHeight;
	}

	SCell& Get(int x, int y)
	{
		return aryMap[y * nScreenWidth + x];
	}
};

class CMinesweeper : public ConsoleGameEngine
{
public:
	CMinesweeper()
	{
		sAppName = L"Minesweeper";
	}

private:
	SWorld world;

private:
	void NewGame(int nMinesCount)
	{
		srand(time(nullptr));

		for (auto& cell : world.aryMap)
		{
			cell.bMine = false;
			cell.bOpen = false;
			cell.bFlag = false;
			cell.nAround = 0;
		}

		world.nTotalMines = nMinesCount;
		world.nClosedCells = nScreenWidth * nScreenHeight;
		world.bFailed = false;

		for (int i = 0; i < nMinesCount; i++)
		{
			int x = rand() % nScreenWidth;
			int y = rand() % nScreenHeight;

			if (world.Get(x, y).bMine) i--;
			else
			{
				world.Get(x, y).bMine = true;

				for (int dx = -1; dx <= 1; dx++)
					for (int dy = -1; dy <= 1; dy++)
					{
						int ox = x + dx;
						int oy = y + dy;

						if (world.Inside(ox, oy))
							world.Get(ox, oy).nAround++;
					}
			}
		}
	}
	
	void OpenFields(int x, int y)
	{
		if (!world.Inside(x, y) || world.Get(x, y).bOpen)
			return;

		world.Get(x, y).bOpen = true;
		world.nClosedCells--;

		if (world.Get(x, y).nAround == 0)
		{
			for (int dx = -1; dx <= 1; dx++)
				for (int dy = -1; dy <= 1; dy++)
					OpenFields(x + dx, y + dy);
		}

		if (world.Get(x, y).bMine)
		{
			world.bFailed = true;

			for (auto& cell : world.aryMap)
				cell.bOpen = true;
		}
	}

protected:
	bool OnUserCreate() override
	{
		NewGame(nMines);

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetMouse(0).bReleased)
		{
			if (world.bFailed)
				NewGame(nMines);
			else
			{
				int x = MouseX();
				int y = MouseY();

				if (world.Inside(x, y) && !world.Get(x, y).bFlag)
					OpenFields(x, y);
			}
		}

		if (GetMouse(1).bReleased)
		{
			int x = MouseX();
			int y = MouseY();

			if (world.Inside(x, y))
			{
				SCell& cell = world.Get(x, y);
				cell.bFlag = !cell.bFlag;
			}
		}

		if (world.nTotalMines == world.nClosedCells)
			NewGame(nMines);

		for (int y = 0; y < nScreenHeight; y++)
			for (int x = 0; x < nScreenWidth; x++)
			{
				SCell& cell = world.Get(x, y);

				if (cell.bOpen)
				{
					if (cell.bMine)
						Draw(x, y, L'X', FG_BLACK | BG_DARK_GREY);
					else if (cell.nAround > 0)
						Draw(x, y, char(cell.nAround + 48), FG_WHITE | BG_GREEN);
					else
						Draw(x, y, PIXEL_SOLID, FG_GREEN);
				}
				else
				{
					if (cell.bFlag)
						Draw(x, y, L'?', FG_RED | BG_DARK_GREY);
					else
						Draw(x, y, PIXEL_SOLID, FG_DARK_GREY);
				}
			}

		if (world.bFailed)
		{
			DrawString(
				(float)nScreenWidth * 0.23f, nScreenHeight / 2,
				L"PRESS LMB TO PLAY AGAIN!", FG_WHITE);
		}

		return true;
	}

};

int main()
{
	CMinesweeper app;

	if (app.ConstructConsole(nScreenWidth, nScreenHeight, 16, 16) == rcode::OK)
		app.Run();

	return 0;
}
