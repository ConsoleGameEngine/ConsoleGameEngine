#include "ConsolaProd.h"

#include <algorithm>

using namespace def;

struct sBall
{
	vf2d pos;
	vf2d vel;

	float radius;

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

	void ConvertBallToRect(vf2d ball_pos, float ball_radius, vf2d& top_left, float& size)
	{
		top_left = ball_pos - 0.5f * ball_radius;

		size = 2.0f * ball_radius;
	}

	bool BallVsRect(vf2d ball_pos, float ball_radius, vf2d rect_pos, vf2d rect_size, Side& s)
	{
		vf2d vTopLeft;
		float fSize;

		ConvertBallToRect(ball_pos, ball_radius, vTopLeft, fSize);

		if (rect_pos.x < vTopLeft.x + fSize || rect_pos.x + rect_size.x > vTopLeft.x)
			s = X_COORD;

		if (rect_pos.y < vTopLeft.y + fSize || rect_pos.y + rect_pos.y > vTopLeft.y)
			s = Y_COORD;
		
		return rect_pos.x < vTopLeft.x + fSize && rect_pos.x + rect_size.x > vTopLeft.x && rect_pos.y <= vTopLeft.y + fSize && rect_pos.y + rect_size.y > vTopLeft.y;
	}

	bool BallVsScreenBounds(vf2d ball_pos, float ball_radius, Side& s)
	{
		vf2d vTopLeft;
		float fSize;

		ConvertBallToRect(ball_pos, ball_radius, vTopLeft, fSize);

		if (vTopLeft.x <= 0.0f || vTopLeft.x + fSize >= (float)GetScreenWidth())
		{
			s = X_COORD;
			return true;
		}

		if (vTopLeft.y <= 0.0f || vTopLeft.y + fSize >= (float)GetScreenHeight())
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

		ball.vel.x = -8.0f;
		ball.vel.y = 8.0f;

		ball.radius = 3.0f;

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
		if (GetKey(VK_LEFT).bHeld)
			platform.pos.x -= 10.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			platform.pos.x += 10.0f * fDeltaTime;

		// Collision detection

		Side unused;
		if (BallVsRect(ball.pos, ball.radius, platform.pos, platform.size, unused))
		{
			ball.vel.y = -ball.vel.y;
			ball.pos.y -= 1.0f;
		}

		Side sb;
		if (BallVsScreenBounds(ball.pos, ball.radius, sb))
		{
			switch (sb)
			{
			case X_COORD: ball.vel.x = -ball.vel.x; break;
			case Y_COORD: ball.vel.y = -ball.vel.y; break;
			}
		}

		// Update Ball position

		ball.pos.x += ball.vel.x * fDeltaTime;
		ball.pos.y += ball.vel.y * fDeltaTime;

		Side pb;
		for (auto& p : listPlatforms)
		{
			if (BallVsRect(ball.pos, ball.radius, p.pos, p.size, pb))
			{
				switch (pb)
				{
				case X_COORD: ball.vel.x = -ball.vel.x; break;
				case Y_COORD: ball.vel.y = -ball.vel.y; break;
				}

				p.should_delete = true;
			}
		}

		listPlatforms.remove_if([](sPlatform& p) { return p.should_delete; });

		// Draw objects

		Clear(PIXEL_SOLID, FG_BLACK);
		
		FillRectangleS(platform.pos, platform.size, PIXEL_SOLID, platform.col);

		FillCircle(ball.pos, ball.radius, PIXEL_SOLID, ball.col);

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
