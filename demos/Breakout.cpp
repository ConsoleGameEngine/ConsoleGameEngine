#include "ConsolaProd.h"

#include <algorithm>

using namespace def;

struct sBall
{
	vf2d pos;
	vf2d size;
	vf2d vel;

	int16_t col = FG_WHITE;
};

struct sPlatform
{
	vf2d pos;
	vf2d size;

	int16_t col = FG_WHITE;

	bool should_delete;
};

enum Side
{
	X_COORD,
	Y_COORD
};

class Example : public def::ConsolaProd
{
public:
	Example()
	{
		sAppName = L"Example";
	}

private:
	sBall ball;
	sPlatform platform;

	std::list<sPlatform> listPlatforms;

	bool bGameActive = true;

	int nScore = 0;

	bool BallVsRect(vf2d ball_pos, vf2d ball_size, vf2d rect_pos, vf2d rect_size, Side& s)
	{
		if (rect_pos.x < ball_pos.x + ball_size.x || rect_pos.x + rect_size.x > ball_pos.x)
			s = X_COORD;

		if (rect_pos.y < ball_pos.y + ball_size.y || rect_pos.y + rect_pos.y > ball_pos.y)
			s = Y_COORD;
		
		return rect_pos.x < ball_pos.x + ball_size.x && rect_pos.x + rect_size.x > ball_pos.x && rect_pos.y <= ball_pos.y + ball_size.y && rect_pos.y + rect_size.y > ball_pos.y;
	}

	bool BallVsScreenBounds(vf2d ball_pos, vf2d ball_size, Side& s)
	{
		if (ball_pos.x <= 0.0f || ball_pos.x + ball_size.x >= (float)GetScreenWidth())
		{
			s = X_COORD;
			return true;
		}

		if (ball_pos.y <= 0.0f || ball_pos.y + ball_size.y >= (float)GetScreenHeight())
		{
			s = Y_COORD;
			return true;
		}

		return false;
	}

protected:
	bool OnUserCreate() override
	{
		ball.pos.x = 0.75f * (float)GetScreenWidth();
		ball.pos.y = 0.5f * (float)GetScreenHeight();

		ball.size.x = 6.0f;
		ball.size.y = 6.0f;

		ball.vel.x = -8.0f;
		ball.vel.y = 8.0f;

		platform.pos.x = 0.5f * (float)GetScreenWidth();
		platform.pos.y = 0.75f * (float)GetScreenHeight();

		platform.size.x = 6.0f;
		platform.size.y = 4.0f;

		int16_t col = FG_WHITE;
		for (int i = 1; i < GetScreenWidth() / 10; i++)
		{
			for (int j = 1; j < 5; j++)
			{
				sPlatform p;

				p.col = col;
				p.pos = vf2d(float(i * 10 - 5), float(j * 8));
				p.size = vf2d(8.0f, 5.0f);
				p.should_delete = false;

				listPlatforms.push_back(p);
			}

			col--;
		}

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		// Check for ending the game

		if (!bGameActive)
		{
			DrawString(int((float)GetScreenWidth() / 2.5f), GetScreenHeight() / 2, L"Game Over!", FG_WHITE);
			return true;
		}

		if (nScore == (GetScreenWidth() / 10 - 1) * 4)
			bGameActive = false;

		// Control player

		if (GetKey(VK_LEFT).bHeld)
			platform.pos.x -= 10.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			platform.pos.x += 10.0f * fDeltaTime;

		// Collision detection

		Side unused;
		if (BallVsRect(ball.pos, ball.size, platform.pos, platform.size, unused))
		{
			ball.vel.y = -ball.vel.y;
			ball.pos.y -= 1.0f;
		}

		Side sb;
		if (BallVsScreenBounds(ball.pos, ball.size, sb))
		{
			switch (sb)
			{
			case X_COORD: ball.vel.x = -ball.vel.x; break;
			case Y_COORD:
				{
					if (ball.pos.y + ball.size.y > GetScreenHeight())
						bGameActive = false;
					else
						ball.vel.y = -ball.vel.y;
					
					break;
				}
			}
		}

		// Update Ball position

		ball.pos.x += ball.vel.x * fDeltaTime;
		ball.pos.y += ball.vel.y * fDeltaTime;

		Side pb;
		bool bCollided = false;

		for (auto& p : listPlatforms)
		{
			if (!bCollided)
			{
				if (BallVsRect(ball.pos, ball.size, p.pos, p.size, pb))
				{
					switch (pb)
					{
					case X_COORD: ball.vel.x = -ball.vel.x; break;
					case Y_COORD: ball.vel.y = -ball.vel.y; break;
					}

					nScore++;

					p.should_delete = true;
					bCollided = true;
				}
			}
		}

		listPlatforms.remove_if([](sPlatform& p) { return p.should_delete; });

		// Draw objects

		Clear(PIXEL_SOLID, FG_BLACK);
		
		FillRectangleS(platform.pos, platform.size, PIXEL_SOLID, platform.col);

		FillRectangleS(ball.pos, ball.size, PIXEL_SOLID, ball.col);

		DrawString(2, 2, L"Score: " + std::to_wstring(nScore), FG_WHITE);

		for (auto& p : listPlatforms)
			FillRectangleS(p.pos, p.size, PIXEL_SOLID, p.col);

		return true;
	}
};

int main()
{
	Example demo;
	def::rcode err = demo.ConstructConsole(160, 100, 8, 8);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
