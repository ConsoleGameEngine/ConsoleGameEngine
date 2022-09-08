#include "ConsoleGameEngine.h"

#include <thread>
#include <list>

using namespace std::chrono_literals;

class Tetris : public def::ConsoleGameEngine
{
public:
	Tetris()
	{
		sAppName = L"Tetris";
	}

private:
	std::string tetromino[7];

	const int nAreaWidth = 12;
	const int nAreaHeight = 18;

	unsigned char* pArea = nullptr;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;

	int nCurrentX = nAreaWidth / 2;
	int nCurrentY = 0;

	int nSpeed = 20;
	int nSpeedCount = 0;

	bool bForceDown = false;

	int nPieceCount = 0;
	int nScore = 0;

	bool bGameOver = false;

	std::list<int> listLines;

protected:
	int Rotate(int px, int py, int r)
	{
		switch (r % 4)
		{
		case 0: return py * 4 + px;
		case 1: return 12 + py - px * 4;
		case 2: return 15 - py * 4 - px;
		case 3: return 3 - py + px * 4;
		}

		return 0;
	}

	bool DoesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
			{
				int p = Rotate(i, j, nRotation);
				int a = (nPosY + j) * nAreaWidth + (nPosX + i);

				if (nPosX + i >= 0 && nPosX + i < nAreaWidth)
					if (nPosY + j >= 0 && nPosY + j < nAreaHeight)
						if (tetromino[nTetromino][p] != L'.' && pArea[a] != 0)
							return false;
			}

		return true;
	}

	bool OnUserCreate() override
	{
		srand(time(0));

		tetromino[0].append("..X...X...X...X.");
		tetromino[1].append("..X..XX...X.....");
		tetromino[2].append(".....XX..XX.....");
		tetromino[3].append("..X..XX..X......");
		tetromino[4].append(".X...XX...X.....");
		tetromino[5].append(".X...X...XX.....");
		tetromino[6].append("..X...X..XX.....");

		pArea = new unsigned char[nAreaWidth * nAreaHeight];

		for (int x = 0; x < nAreaWidth; x++)
			for (int y = 0; y < nAreaHeight; y++)
				pArea[y * nAreaWidth + x] = (x == 0 || x == nAreaWidth - 1 || y == nAreaHeight - 1) ? 9 : 0;

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::PIXEL_SOLID, def::FG_BLACK);

		if (bGameOver)
			DrawString(5, 10, L"Game over");
		else
		{

			// MAKE A DELAY

			std::this_thread::sleep_for(50ms);

			nSpeedCount++;
			bForceDown = (nSpeedCount == nSpeed);

			// PLAYER MOVEMENT
			
			nCurrentX += (GetKey(VK_RIGHT).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
			nCurrentX -= (GetKey(VK_LEFT).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
			nCurrentY += (GetKey(VK_DOWN).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

			if (GetMouse(0).bPressed)
				nCurrentRotation += DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY) ? 1 : 0;

			if (bForceDown)
			{
				nSpeedCount = 0;
				nPieceCount++;

				if (nPieceCount % 50 == 0)
					if (nSpeed >= 10) nSpeed--;

				if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
					nCurrentY++;
				else
				{
					for (int px = 0; px < 4; px++)
						for (int py = 0; py < 4; py++)
							if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
								pArea[(nCurrentY + py) * nAreaWidth + (nCurrentX + px)] = nCurrentPiece + 1;

					for (int py = 0; py < 4; py++)
						if (nCurrentY + py < nAreaHeight - 1)
						{
							bool bLine = true;

							for (int px = 1; px < nAreaWidth - 1; px++)
								bLine &= (pArea[(nCurrentY + py) * nAreaWidth + px]) != 0;

							if (bLine)
							{
								for (int px = 1; px < nAreaWidth - 1; px++)
									pArea[(nCurrentY + py) * nAreaWidth + px] = 8;
								listLines.push_back(nCurrentY + py);
							}
						}

					nScore += 25;
					if (!listLines.empty())	nScore += (1 << listLines.size()) * 100;

					nCurrentX = nAreaWidth / 2;
					nCurrentY = 0;
					nCurrentRotation = 0;
					nCurrentPiece = rand() % 7;

					bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
				}
			}


			// DRAW AREA

			for (int i = 0; i < nAreaWidth; i++)
				for (int j = 0; j < nAreaHeight; j++)
					Draw(i + 2, j + 2, def::PIXEL_SOLID, pArea[j * nAreaWidth + i]);

			// DRAW CURRENT PIECE

			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 4; j++)
					if (tetromino[nCurrentPiece][Rotate(i, j, nCurrentRotation)] != '.')
						Draw(nCurrentX + i + 2, nCurrentY + j + 2, def::PIXEL_SOLID, nCurrentPiece + 1);

			// DISPLAY SCORE

			DrawString(15, 1, L"Score:" + std::to_wstring(nScore));

			// ANIMATE LINES

			if (!listLines.empty())
			{
				//std::this_thread::sleep_for(400ms);

				for (auto& l : listLines)
					for (int i = 1; i < nAreaWidth - 1; i++)
					{
						for (int j = l; j > 0; j--)
							pArea[j * nAreaWidth + i] = pArea[(j - 1) * nAreaWidth + i];

						pArea[i] = 0;
					}

				listLines.clear();
			}
		}

		return true;
	}

};

int main()
{
	Tetris demo;
	def::rcode err = demo.ConstructConsole(80, 50, 16, 16);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
