#include "ConsolaProd.h"

enum class CELL_TYPE
{
	CIRCLE,
	CROSS,
	BLANK
};

class TicTacToe : public def::ConsolaProd
{
public:
	TicTacToe()
	{
		sAppName = L"Tic-Tac-Toe";
	}

private:
	const int nCellWidth = 20;
	const int nCellHeight = 20;

	static const int nCellXCount = 3;
	static const int nCellYCount = 3;

	CELL_TYPE rgCells[nCellXCount * nCellYCount];

	bool bPlayer = 0;
	bool bEnd = false;

	bool CheckDiagonal(CELL_TYPE player)
	{
		bool bToLeft = true, bToRight = true;

		for (int i = 0; i < nCellXCount; i++)
		{
			bToRight &= (rgCells[i * nCellXCount + i] == player);
			bToLeft &= (rgCells[(nCellXCount - i - 1) * nCellXCount + i] == player);
		}

		return bToLeft || bToRight;
	}

	bool CheckLines(CELL_TYPE player)
	{
		bool bCols, bRows;

		for (int x = 0; x < nCellXCount; x++)
		{
			bCols = true;
			bRows = true;
			for (int y = 0; y < nCellYCount; y++)
			{
				bCols &= (rgCells[y * nCellXCount + x] == player);
				bRows &= (rgCells[x * nCellXCount + y] == player);
			}

			if (bCols || bRows) return true;
		}

		return false;
	}

	bool IsWinner()
	{
		return CheckDiagonal(CELL_TYPE(bPlayer)) || CheckLines(CELL_TYPE(bPlayer));
	}

protected:
	bool OnUserCreate() override
	{
		for (auto& c : rgCells)
			c = CELL_TYPE::BLANK;

		Clear(def::Pixel::SOLID, def::FG::WHITE);

		for (int i = 0; i <= nCellXCount; i++)
			for (int j = 0; j <= nCellYCount; j++)
				DrawRectangle(i * nCellWidth, j * nCellHeight, nCellWidth, nCellHeight, def::Pixel::SOLID, def::FG::BLACK);


		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (!bEnd)
		{
			if (GetMouseBtn(0).bPressed)
			{
				int mx = GetMouseX();
				int my = GetMouseY();

				int cx = mx / nCellWidth;
				int cy = my / nCellHeight;

				if (rgCells[cy * nCellXCount + cx] == CELL_TYPE::BLANK)
					rgCells[cy * nCellXCount + cx] = CELL_TYPE(bPlayer = !bPlayer);

				if (IsWinner())
				{
					std::wstring winner = (CELL_TYPE(bPlayer) == CELL_TYPE::CIRCLE) ? L"O won" :  L"X won";
					DrawString(nCellWidth / 3 + 1, GetScreenHeight() / 2 + nCellHeight / 3, winner, def::Pixel::SOLID, def::FG::BLACK);
					bEnd = true;
				}
			}

			for (int i = 0; i < nCellXCount; i++)
				for (int j = 0; j < nCellYCount; j++)
				{
					switch (rgCells[j * nCellYCount + i])
					{
					case CELL_TYPE::BLANK: break;
					case CELL_TYPE::CROSS:
						DrawLine(i * nCellWidth + 1, j * nCellHeight + 1, i * nCellWidth + nCellWidth - 1, j * nCellHeight + nCellHeight - 1, def::Pixel::SOLID, def::FG::BLACK);
						DrawLine(i * nCellWidth + nCellWidth - 1, j * nCellHeight + 1, i * nCellWidth + 1, j * nCellHeight + nCellHeight - 1, def::Pixel::SOLID, def::FG::BLACK);
						break;
					case CELL_TYPE::CIRCLE:
						DrawCircle(i * nCellWidth + nCellWidth / 2, j * nCellHeight + nCellHeight / 2, nCellWidth / 2 - 1, def::Pixel::SOLID, def::FG::BLACK);
						break;
					}
				}
		}

		return true;
	}
};

int main()
{
	TicTacToe game;

	def::rcode err = game.ConstructConsole(180, 120, 8, 8);

	if (err.ok)
		game.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
