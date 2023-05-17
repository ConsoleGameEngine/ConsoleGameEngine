#define DGE_APPLICATION
#include "defGameEngine.h"

#include <thread>

using namespace std;

class CellularAutomata : public def::GameEngine
{
public:
	CellularAutomata()
	{
		SetTitle("Cellular Automata");
	}

private:
	bool* output = nullptr;
	bool* state = nullptr;

	float fTicks = 0.0f;

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

		/*for (int i = 0; i < ScreenWidth() * ScreenHeight(); i++)
			state[i] = (bool)(rand() % 2);*/

		auto set = [&](int x, int y, std::string v)
		{
			for (int i = 0; i < v.size(); i++)
				state[y * ScreenWidth() + (x + i)] = (v[i] == '#') ? true : false;
		};

		set(60, 45, "........................#............");
		set(60, 46, "......................#.#............");
		set(60, 47, "............##......##............##.");
		set(60, 48, "...........#...#....##............##.");
		set(60, 49, "##........#.....#...##...............");
		set(60, 50, "##........#...#.##....#.#............");
		set(60, 51, "..........#.....#.......#............");
		set(60, 52, "...........#...#.....................");
		set(60, 53, "............##.......................");

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetKey(def::Key::SPACE).bHeld)
			return true;

		fTicks += fDeltaTime;
		if (fTicks > 0.1f)
		{
			fTicks = 0.0f;

			Clear(def::BLACK);

			DrawRectangle(0, 0, ScreenWidth() - 1, ScreenHeight() - 1, def::RED);

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
						Draw(x, y, def::WHITE);

				}
		}

		return true;
	}
};

int main()
{
	srand(clock());

	CellularAutomata game;

	if (game.Construct(160, 100, 8, 8))
		game.Run();

	return 0;
}
