#include "ConsoleGameEngine.h"

using namespace std;

class CellularAutomata : public ConsoleGameEngine
{
public:
	CellularAutomata()
	{
		sAppName = L"Cellular Automata";
	}

private:
	bool* output = nullptr;
	bool* state = nullptr;

private:
	int GetNeighboursCount(int x, int y)
	{
		int nCount = 0;

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
				nCount += output[(y + j) * ScreenWidth() + (x + i)];
		}

		if (output[y * ScreenWidth() + x] == 1) nCount--;

		return nCount;
	}

protected:
	bool OnUserCreate() override
	{
		output = new bool[ScreenWidth() * ScreenHeight()];
		state = new bool[ScreenWidth() * ScreenHeight()];

		memset(output, 0, ScreenWidth() * ScreenHeight() * sizeof(bool));
		memset(state, 0, ScreenWidth() * ScreenHeight() * sizeof(bool));

		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			state[i] = (bool)(rand() % 2);

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(VK_SPACE).bHeld)
			return true;

		Clear(PIXEL_SOLID, FG_BLACK);

		DrawRectangle(0, 0, ScreenWidth() - 1, ScreenHeight() - 1, PIXEL_SOLID, FG_RED);

		//using namespace chrono_literals;
		//this_thread::sleep_for(75ms);

		for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			output[i] = state[i];

		for (int x = 1; x < ScreenWidth() - 1; x++)
			for (int y = 1; y < ScreenHeight() - 1; y++)
			{
				int nCount = GetNeighboursCount(x, y);

				if (output[y * ScreenWidth() + x] == 1)
					state[y * ScreenWidth() + x] = (nCount == 2 || nCount == 3);
				else
					state[y * ScreenWidth() + x] = (nCount == 3);

				if (output[y * ScreenWidth() + x] == 1)
					Draw(x, y, PIXEL_SOLID, FG_WHITE);

			}

		return true;
	}
};

int main()
{
	srand(clock());

	CellularAutomata game;

	if (game.ConstructConsole(160, 100, 8, 8) == RCODE_OK)
		game.Run();

	return 0;
}
