#include "ConsoleGameEngine.h"

#include <thread>
#include <list>

using namespace std;
using namespace chrono_literals;

class Tetris : public ConsoleGameEngine
{
public:
	Tetris()
	{
		sAppName = L"Tetris";
	}

private:
	string tetromino[7];

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

	list<int> listLines;

protected:
	int Rotate(int x, int y, int r)
	{
		switch (r % 4)
		{
		case 0: return y * 4 + x;
		case 1: return 12 + y - x * 4;
		case 2: return 15 - y * 4 - x;
		case 3: return 3 - y + x * 4;
		}

		return 0;
	}

	bool DoesPieceFit(int t, int r, int x, int y)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				int p = Rotate(i, j, r);
				int a = (y + j) * nAreaWidth + (x + i);

				if (x + i >= 0 && x + i < nAreaWidth &&
					y + j >= 0 && y + j < nAreaHeight)
				{
					if (tetromino[t][p] != L'.' && pArea[a] != 0)
						return false;
				}
			}
		}

		return true;
	}

	virtual bool OnUserCreate() override
	{
		srand(time(0));

		tetromino[0] = "..X...X...X...X.";
		tetromino[1] = "..X..XX...X.....";
		tetromino[2] = ".....XX..XX.....";
		tetromino[3] = "..X..XX..X......";
		tetromino[4] = ".X...XX...X.....";
		tetromino[5] = ".X...X...XX.....";
		tetromino[6] = "..X...X..XX.....";

		pArea = new unsigned char[nAreaWidth * nAreaHeight];

		for (int x = 0; x < nAreaWidth; x++)
		{
			for (int y = 0; y < nAreaHeight; y++)
				pArea[y * nAreaWidth + x] = (x == 0 || x == nAreaWidth - 1 || y == nAreaHeight - 1) ? 9 : 0;
		}

		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(PIXEL_SOLID, FG_BLACK);

		if (bGameOver)
		{
			DrawString(5, 10, L"Game over");
			return true;
		}

		this_thread::sleep_for(50ms);

		nSpeedCount++;
		bForceDown = (nSpeedCount == nSpeed);

		nCurrentX += (int)(GetKey(VK_RIGHT).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY));
		nCurrentX -= (int)(GetKey(VK_LEFT).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY));
		nCurrentY += (int)(GetKey(VK_DOWN).bHeld && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1));

		if (GetMouse(0).bPressed)
			nCurrentRotation += (int)DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY);

		if (bForceDown)
		{
			nSpeedCount = 0;
			nPieceCount++;

			if (nPieceCount % 50 == 0 && nSpeed >= 10)
				nSpeed--;

			if (DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else
			{
				for (int px = 0; px < 4; px++)
				{
					for (int py = 0; py < 4; py++)
					{
						if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
							pArea[(nCurrentY + py) * nAreaWidth + (nCurrentX + px)] = nCurrentPiece + 1;
					}
				}

				for (int py = 0; py < 4; py++)
				{
					if (nCurrentY + py < nAreaHeight - 1)
					{
						bool bLine = true;

						for (int px = 1; px < nAreaWidth - 1; px++)
							bLine &= ((pArea[(nCurrentY + py) * nAreaWidth + px]) != 0);

						if (bLine)
						{
							for (int px = 1; px < nAreaWidth - 1; px++)
								pArea[(nCurrentY + py) * nAreaWidth + px] = 8;

							listLines.push_back(nCurrentY + py);
						}
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

		for (int i = 0; i < nAreaWidth; i++)
		{
			for (int j = 0; j < nAreaHeight; j++)
				Draw(i + 2, j + 2, PIXEL_SOLID, pArea[j * nAreaWidth + i]);
		}
				

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (tetromino[nCurrentPiece][Rotate(i, j, nCurrentRotation)] != '.')
					Draw(nCurrentX + i + 2, nCurrentY + j + 2, PIXEL_SOLID, nCurrentPiece + 1);
			}
		}

		DrawString(15, 1, L"Score:" + to_wstring(nScore));

		if (!listLines.empty())
		{
			for (auto& l : listLines)
				for (int i = 1; i < nAreaWidth - 1; i++)
				{
					for (int j = l; j > 0; j--)
						pArea[j * nAreaWidth + i] = pArea[(j - 1) * nAreaWidth + i];

					pArea[i] = 0;
				}

			listLines.clear();
		}

		return true;
	}

};

int main()
{
	Tetris demo;

	if (demo.ConstructConsole(80, 50, 16, 16) == RCODE_OK)
		demo.Run();

	return 0;
}
