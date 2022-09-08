#include "ConsoleGameEngine.h"

using namespace def;

class CellularAutomata : public def::ConsoleGameEngine
{
public:
	CellularAutomata()
	{
		sAppName = L"Cellular Automata";
	}

private:
	bool* output;
	bool* state;

private:
	int GetNeighboursCount(int x, int y)
	{
		int nCount = 0;

		for (int i = -1; i <= 1; i++)
			for (int j = -1; j <= 1; j++)
				nCount += output[(y + j) * GetScreenWidth() + (x + i)];

		// Because we don't count center cell
		if (output[y * GetScreenWidth() + x] == 1)
			nCount--;

		return nCount;
	}

protected:
	bool OnUserCreate() override
	{
		output = new bool[GetScreenWidth() * GetScreenHeight()];
		state = new bool[GetScreenWidth() * GetScreenHeight()];

		memset(output, 0, GetScreenWidth() * GetScreenHeight() * sizeof(bool));
		memset(state, 0, GetScreenWidth() * GetScreenHeight() * sizeof(bool));

		for (int i = 0; i < GetScreenWidth() * GetScreenHeight(); i++)
			state[i] = rand() % 2;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(VK_SPACE).bHeld)
			return true;

		Clear(PIXEL_SOLID, FG_BLACK);

		DrawRectangle(0, 0, GetScreenWidth() - 1, GetScreenHeight() - 1, PIXEL_SOLID, FG_RED);

		// Make a delay to see work of algorithm
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(75ms);

		for (int i = 0; i < GetScreenWidth() * GetScreenHeight(); i++)
			output[i] = state[i];

		for (int x = 1; x < GetScreenWidth() - 1; x++)
			for (int y = 1; y < GetScreenHeight() - 1; y++)
			{
				int nCount = GetNeighboursCount(x, y);

				if (output[y * GetScreenWidth() + x] == 1)
					state[y * GetScreenWidth() + x] = (nCount == 2 || nCount == 3);
				else
					state[y * GetScreenWidth() + x] = (nCount == 3);

				if (output[y * GetScreenWidth() + x] == 1)
					Draw(x, y, PIXEL_SOLID, FG_WHITE);

			}

		return true;
	}
};

int main()
{
	srand(clock());

	CellularAutomata game;

	def::rcode err = game.ConstructConsole(160, 100, 8, 8);

	if (err.ok)
		game.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
