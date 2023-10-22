#define CGE_IMPL
#include "ConsoleGameEngine.hpp"

struct Object
{
	float px, py;
	float vx, vy;
	float sx, sy;
};

class CPong : public ConsoleGameEngine
{
public:
	CPong()
	{
		sAppName = L"Pong";
	}

private:
	Object bats[2];
	Object ball;

	int score[2];

protected:
	static bool RectVsRect(
		float px1, float py1, float sx1, float sy1,
		float px2, float py2, float sx2, float sy2)
	{
		return px1 + sx1 >= px2 &&
			px1 <= px2 + sx2 &&
			py1 + sy1 >= py2 &&
			py1 <= py2 + sy2;
	}

	void ResetBall()
	{
		ball.px = float(ScreenWidth() / 2);
		ball.py = float(ScreenHeight() / 2);

		ball.vx = -1.0f;
		ball.vy = 1.0f;
	}

	bool OnUserCreate() override
	{
		bats[0].px = 2.0f;
		bats[1].px = float(ScreenWidth() - 3);

		for (int i = 0; i < 2; i++)
		{
			bats[i].py = (float)ScreenHeight() * 0.3f;
			bats[i].sx = 1.0f;
			bats[i].sy = (float)ScreenHeight() * 0.2f;

			score[i] = 0;
		}

		ResetBall();

		ball.sx = 2.0f;
		ball.sy = 2.0f;

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (GetKey(L'W').bHeld) bats[0].vy = -1.0f;
		if (GetKey(L'S').bHeld) bats[0].vy = +1.0f;

		if (GetKey(VK_UP).bHeld) bats[1].vy = -1.0f;
		if (GetKey(VK_DOWN).bHeld) bats[1].vy = +1.0f;

		for (int i = 0; i < 2; i++)
		{
			if (RectVsRect(bats[i].px, bats[i].py, bats[i].sx, bats[i].sy,
				ball.px, ball.py, ball.sx, ball.sy))
			{
				ball.vx *= -1.0f;
			}

			bats[i].px += 50.0f * bats[i].vx * fDeltaTime;
			bats[i].py += 50.0f * bats[i].vy * fDeltaTime;

			bats[i].vx = 0.0f;
			bats[i].vy = 0.0f;
		}

		ball.px += 20.0f * ball.vx * fDeltaTime;
		ball.py += 20.0f * ball.vy * fDeltaTime;

		if (ball.py + ball.sy >= ScreenHeight() || ball.py < 0.0f)
		{
			ball.vy *= -1.0f;
			ball.py += ball.vy;
		}

		if (ball.px < 0.0f)
		{
			score[1]++;
			ResetBall();
		}

		if (ball.px >= ScreenWidth())
		{
			score[0]++;
			ResetBall();
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		for (int y = 0; y < ScreenHeight(); y++)
		{
			if (y % 2 == 0)
				Draw(ScreenWidth() / 2, y, PIXEL_SOLID, FG_WHITE);
		}

		DrawString(1, 1, std::to_wstring(score[0]), FG_WHITE);

		std::wstring score2 = std::to_wstring(score[1]);
		DrawString(ScreenWidth() - score2.length() - 1, 1, score2, FG_WHITE);

		for (int i = 0; i < 2; i++)
			FillRectangle(bats[i].px, bats[i].py, bats[i].sx, bats[i].sy, PIXEL_SOLID, FG_YELLOW);

		FillRectangle(ball.px, ball.py, ball.sx, ball.sy, PIXEL_HALF, FG_RED);

		return true;
	}

};

int main()
{
	CPong app;

	if (app.ConstructConsole(80, 40, 12, 12) == rcode::OK)
		app.Run();

	return 0;
}
