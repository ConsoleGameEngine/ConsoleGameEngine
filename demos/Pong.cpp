#include "ConsoleGameEngine.h"

struct sBall
{
	float x;
	float y;

	int radius;

	float speed;

	float xVel;
	float yVel;
};

struct sPlayer
{
	float x;
	float y;

	int width;
	int height;

	float speed;
};

class AtariPong : public def::ConsoleGameEngine
{
public:
	AtariPong()
	{
		sAppName = L"Atari Pong";
	}

private:
	sBall ball;

	int nPlayer1_Score;
	int nPlayer2_Score;

	sPlayer player1;
	sPlayer player2;

protected:

	void Ball_Move()
	{
		ball.x += ball.xVel * GetDeltaTime();
		ball.y += ball.yVel * GetDeltaTime();
	}

	void Ball_Reset(float fModifier)
	{
		ball.x = float(GetScreenWidth() / 2 - ball.radius / 2);
		ball.y = float(GetScreenHeight() / 2 - ball.radius / 2);

		ball.xVel = ball.speed * fModifier;
		ball.yVel = 0.0f;
	}

	void Ball_ChangeScore()
	{
		if (ball.x < 0.0f)
		{
			Ball_Reset(1.0f);
			nPlayer2_Score++;
		}
		else if ((int)ball.x + ball.radius > GetScreenWidth())
		{
			Ball_Reset(-1.0f);
			nPlayer1_Score++;
		}
	}

	void Ball_CollideWall()
	{
		if (ball.y < 0.0f)
		{
			ball.y = 0.0f;
			ball.yVel = -ball.yVel;
		}
		else if ((int)ball.y + ball.radius > GetScreenHeight())
		{
			ball.y = float(GetScreenHeight() - ball.radius);
			ball.yVel = -ball.yVel;
		}
	}

	void Ball_CollidePlayer(sPlayer& p, int dir)
	{
		if (CheckCollision(ball, p))
		{
			ball.xVel = ball.speed * dir;

			float fMiddleBall = ball.y + float(ball.radius / 2);
			float fMiddlePlayer = float(p.y + p.height / 2);

			ball.yVel = (fMiddleBall - fMiddlePlayer) * 5.0f;
		}
	}

	void Ball_Collide()
	{
		Ball_CollideWall();

		Ball_CollidePlayer(player1, 1);
		Ball_CollidePlayer(player2, -1);
	}

	void Ball_Draw()
	{
		FillCircle((int)ball.x, (int)ball.y, ball.radius);
	}

	void Player_Move(sPlayer& p, const short upKey, const short downKey)
	{
		if (GetKey(upKey).bHeld)
			p.y -= p.speed * GetDeltaTime();

		if (GetKey(downKey).bHeld)
			p.y += p.speed * GetDeltaTime();
	}

	void Player_Clip(sPlayer& p)
	{
		if (p.y < 0.0f)
			p.y = 1.0f;
		else if (p.y + (float)p.height > GetScreenHeight())
			p.y = float(GetScreenHeight() - p.height) - 1.0f;
	}

	void Player_Draw(sPlayer& p)
	{
		FillRectangle((int)p.x, (int)p.y, (int)p.x + p.width, (int)p.y + p.height);
	}

	bool CheckCollision(sBall& b, sPlayer& p)
	{
		float fBallX = b.x - float(b.radius / 2);
		float fBallY = b.y - float(b.radius / 2);

		float fSize = float(b.radius * 2);

		return p.x < fBallX + fSize - 1 && p.x + (float)p.width + 1 > fBallX && p.y < fBallY + fSize - 1 && p.y + (float)p.height + 1 > fBallY;
	}

	void ResetGame()
	{
		// Initialize a ball

		ball.x = float(GetScreenWidth() / 2);
		ball.y = float(GetScreenHeight() / 2);

		ball.radius = 3;

		ball.speed = 20.0f;

		ball.xVel = -ball.speed;
		ball.yVel = 0.0f;

		// Initialize the players

		player1.x = 50.0f;
		player1.y = float(GetScreenHeight() / 2);

		player1.width = 5;
		player1.height = 50;

		player1.speed = 50.0f;

		player2.width = 5;
		player2.height = 50;

		player2.x = float(GetScreenWidth() - player2.width - 50);
		player2.y = float(GetScreenHeight() / 2);

		player2.speed = 50.0f;

		// Initialize score

		nPlayer1_Score = 0;
		nPlayer2_Score = 0;
	}

	bool OnUserCreate() override
	{
		ResetGame();

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::PIXEL_SOLID, def::FG_BLACK);

		// Update ball

		Ball_Move();
		Ball_Collide();
		Ball_ChangeScore();
		Ball_Draw();

		// Update players

		Player_Move(player1, L'W', L'S');
		Player_Clip(player1);
		Player_Draw(player1);

		Player_Move(player2, VK_UP, VK_DOWN);
		Player_Clip(player2);
		Player_Draw(player2);

		// Update score

		if (nPlayer1_Score == 5 || nPlayer2_Score == 5)
			ResetGame();

		DrawString(10, 10, L"Score: " + std::to_wstring(nPlayer1_Score));
		DrawString(GetScreenWidth() - 75, 10, L"Score: " + std::to_wstring(nPlayer2_Score));

		return true;
	}

};

int main()
{
	AtariPong demo;

	def::rcode rc = demo.ConstructConsole(256, 240, 4, 4);

	if (rc.ok)
		demo.Run();
	else
		std::cerr << rc.info << '\n';

	return 0;
}
