#define CONSOLE_GAME_ENGINE_IMPLEMENTATION
#include "ConsoleGameEngine.hpp"

#include <list>

using namespace std;

class FlappyBird : public ConsoleGameEngine
{
public:
	FlappyBird()
	{
		sAppName = L"Flappy Bird";
	}

	float fBirdPosition;
	float fBirdVelocity;
	float fBirdAcceleration;

	float fSectionWidth;
	list<int> listSections;

	int nScore;
	int nBestScore;

	float fLevelPosition;
	bool bGameOver;

	const float fGravity = 100.0f;

private:
	void Reset()
	{
		srand(time(0));

		fBirdPosition = (float)ScreenHeight() * 0.5f;
		fBirdVelocity = 0.0f;
		fBirdAcceleration = 0.0f;

		listSections = { 0, 0, 0, 0 };
		fSectionWidth = (float)ScreenWidth() / float(listSections.size() - 1);

		nScore = 0;

		fLevelPosition = 0.0f;
		bGameOver = false;
	}

	bool OnUserCreate() override
	{
		Reset();
		nBestScore = 0;

		return true;
	}

	bool RectVsRect(float r1_x1, float r1_y1, float r1_x2, float r1_y2, float r2_x1, float r2_y1, float r2_x2, float r2_y2)
	{
		return r1_x1 <= r2_x2 && r1_y1 <= r2_y2 && r1_x2 >= r2_x1 && r1_y2 >= r2_y1;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (bGameOver)
		{
			if (GetKey(VK_RETURN).bPressed)
				Reset();

			return true;
		}

		if (GetKey(VK_SPACE).bPressed && fBirdVelocity >= fGravity * 0.1f)
		{
			fBirdAcceleration = 0.0f;
			fBirdVelocity = -fGravity * 0.25f;
		}
		else
			fBirdAcceleration += fGravity * fDeltaTime;

		if (fBirdAcceleration >= fGravity)
			fBirdAcceleration = fGravity;

		fBirdVelocity += fBirdAcceleration * fDeltaTime;
		fBirdPosition += fBirdVelocity * fDeltaTime;

		fLevelPosition += 14.0f * fDeltaTime;

		if (fLevelPosition > fSectionWidth)
		{
			fLevelPosition -= fSectionWidth;

			if (listSections.front() != 0)
				nScore++;

			listSections.pop_front();
			
			int nSize = rand() % int((float)ScreenHeight() * 0.7f);
			if (nSize <= int((float)ScreenHeight() * 0.17f)) nSize = 0;

			listSections.push_back(nSize);
		}

		list<pair<pair<int, int>, pair<int, int>>> listObstacles;

		int nSection = 0;
		for (int s : listSections)
		{
			if (s != 0)
			{
				float fOffset = (float)nSection * fSectionWidth - fLevelPosition;

				float fPos1 = fOffset + (float)ScreenWidth() * 0.125f;
				float fPos2 = fOffset + (float)ScreenWidth() * 0.1875f;

				listObstacles.push_back({
					{ (int)fPos1, ScreenHeight() - s }, { (int)fPos2, ScreenHeight() }
				});

				listObstacles.push_back({
					{ (int)fPos1, 0 }, { (int)fPos2, ScreenHeight() - s - ScreenHeight() / 4 }
				});
			}

			nSection++;
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		int nBirdX = ScreenWidth() / 3;

		int nBirdWidth = 5;
		int nBirdHeight = 5;

		if (fBirdVelocity > 0.0f)
		{
			DrawString(nBirdX, (int)fBirdPosition + 0, LR"(\\\)", FG_WHITE);
			DrawString(nBirdX, (int)fBirdPosition + 1, LR"(<\\\=&)", FG_WHITE);
		}
		else
		{
			DrawString(nBirdX, (int)fBirdPosition + 0, L"<///=&", FG_WHITE);
			DrawString(nBirdX, (int)fBirdPosition + 1, L"///", FG_WHITE);
		}

		for (const auto& o : listObstacles)
		{
			if (RectVsRect(
				(float)nBirdX, fBirdPosition, (float)nBirdX + 6, fBirdPosition + 2,
				o.first.first, o.first.second, o.second.first, o.second.second))
			{
				if (nBestScore < nScore)
					nBestScore = nScore;

				bGameOver = true;
			}

			FillRectangle(
				o.first.first, o.first.second,
				o.second.first - o.first.first, o.second.second - o.first.second,
				PIXEL_SOLID, FG_GREEN);
		}

		DrawString(1, 1, L"Score: " + to_wstring(nScore), FG_WHITE);
		DrawString(1, 2, L"Best: " + to_wstring(nBestScore), FG_WHITE);

		if (bGameOver)
			DrawString(int((float)ScreenWidth() * 0.4f), ScreenHeight() / 2, L"PRESS ENTER TO PLAY AGAIN", FG_WHITE);

		return true;
	}

};

int main()
{
	FlappyBird app;

	if (app.ConstructConsole(80, 60, 16, 16) == RC_OK)
		app.Run();

	return 0;
}
