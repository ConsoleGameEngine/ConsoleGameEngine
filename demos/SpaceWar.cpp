#define CONSOLE_GAME_ENGINE_IMPLEMENTATION
#include "ConsoleGameEngine.hpp"

#include <list>

using namespace std;

class Space : public ConsoleGameEngine
{
public:
	Space()
	{
		sAppName = L"Space In War";
	}

	int nPlayerWidth;
	int nPlayerHeight;
	wstring sPlayerModel;

	int nEnemyWidth;
	int nEnemyHeight;
	wstring sEnemyModel;

	struct Object
	{
		pair<float, float> pos;
		bool bRedundant = false;
	};

	list<Object> listEnemies;
	list<Object> listProjectiles;

	float fPlayerX;
	float fPlayerY;

	int nScore;

	bool bGameOver;

	float fSpeed;

	vector<wstring> vPatterns;
	int nCurrentPattern;

protected:
	bool OnUserCreate() override
	{
		srand(time(NULL));

		nPlayerWidth = 9;
		nPlayerHeight = 7;
		sPlayerModel += L"<----->  ";
		sPlayerModel += L" |       ";
		sPlayerModel += L" @@@\\    ";
		sPlayerModel += L"-###=)   ";
		sPlayerModel += L" @@@/    ";
		sPlayerModel += L" |       ";
		sPlayerModel += L"<----->  ";

		nEnemyWidth = 9;
		nEnemyHeight = 5;
		sEnemyModel += L"<------->";
		sEnemyModel += L"      |  ";
		sEnemyModel += L" <#####=-";
		sEnemyModel += L"      |  ";
		sEnemyModel += L"<------->";

		wstring sPattern;

		sPattern = L".........";
		sPattern += L".........";
		sPattern += L".........";
		sPattern += L"#........";
		sPattern += L".........";
		sPattern += L".........";
		sPattern += L".........";
		vPatterns.push_back(sPattern);

		sPattern = L".........";
		sPattern += L".........";
		sPattern += L"#........";
		sPattern += L".........";
		sPattern += L"#........";
		sPattern += L".........";
		sPattern += L".........";
		vPatterns.push_back(sPattern);

		sPattern = L".........";
		sPattern += L".........";
		sPattern += L"..#......";
		sPattern += L"#........";
		sPattern += L"..#......";
		sPattern += L".........";
		sPattern += L".........";
		vPatterns.push_back(sPattern);

		sPattern = L".........";
		sPattern += L"....#....";
		sPattern += L"..#......";
		sPattern += L"#...#....";
		sPattern += L"..#......";
		sPattern += L"....#....";
		sPattern += L".........";
		vPatterns.push_back(sPattern);

		fSpeed = 20.0f;

		Restart();

		return true;
	}

	void Restart()
	{
		listEnemies.clear();
		listProjectiles.clear();

		for (int i = 0; i < 3; i++)
			AddEnemy();

		fPlayerX = ScreenWidth() / 4;
		fPlayerY = ScreenHeight() / 2 - nPlayerHeight / 2;

		nScore = 0;

		nCurrentPattern = 0;

		bGameOver = false;
	}

	void AddEnemy()
	{
		listEnemies.push_back({ make_pair<float, float>(
			ScreenWidth() + nEnemyWidth + (rand() % (nEnemyWidth * 2)),
			RandInt(nEnemyHeight, ScreenHeight() - nEnemyHeight)
		) });
	}

	int RandInt(int min, int max)
	{
		return min + rand() % (max - min);
	}

	bool RectVsRect(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2)
	{
		return x1 < x2 + w2 && x1 + w1 > x2 && y1 <	y2 + h2 && y1 + h1 > y2;
	}

	bool RectVsPoint(float x1, float y1, float w, float h, float x2, float y2)
	{
		return x1 <= x2 && x2 < x1 + w && y1 <= y2 && y2 < y1 + h;
	}

	void AddProjectiles(size_t i)
	{
		wstring& p = vPatterns[i];

		for (int i = 0; i < nPlayerWidth; i++)
			for (int j = 0; j < nPlayerHeight; j++)
			{
				if (p[j * nPlayerWidth + i] == '#')
				{
					listProjectiles.push_back({ {
						fPlayerX + float(nPlayerWidth + i),
						fPlayerY + float(j)
					} });
				}
			}
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (bGameOver)
		{
			if (GetKey(VK_SPACE).bPressed)
				Restart();

			Clear(PIXEL_SOLID, FG_BLACK);

			std::wstring sMsg = L"Game Over! Score: " + to_wstring(nScore) + L". Press SPACE to play again.";
			DrawString(ScreenWidth() / 2 - sMsg.length() / 2, ScreenHeight() / 2, sMsg, FG_WHITE);

			return true;
		}

		if (GetKey(VK_UP).bHeld) fPlayerY -= fSpeed * fDeltaTime;
		if (GetKey(VK_DOWN).bHeld) fPlayerY += fSpeed * fDeltaTime;

		if (GetKey(VK_RETURN).bPressed) ++nCurrentPattern %= vPatterns.size();

		if (GetKey(VK_SPACE).bPressed)
			AddProjectiles(nCurrentPattern);

		for (auto& p : listEnemies)
		{
			p.pos.first -= fSpeed * fDeltaTime;

			if (RectVsRect(p.pos.first, p.pos.second, nEnemyWidth, nEnemyHeight, fPlayerX, fPlayerY, nPlayerWidth, nPlayerHeight))
				bGameOver = true;
		}

		for (auto& proj : listProjectiles)
		{
			proj.pos.first += 3.0f * fSpeed * fDeltaTime;
			proj.bRedundant = (proj.pos.first >= ScreenWidth());

			for (auto& enemy : listEnemies)
			{
				enemy.bRedundant = (enemy.pos.first <= -nEnemyWidth);

				if (RectVsPoint(enemy.pos.first, enemy.pos.second, nEnemyWidth, nEnemyHeight, proj.pos.first, proj.pos.second))
				{
					proj.bRedundant = enemy.bRedundant = true;
					AddEnemy();
					nScore++;
				}
			}
		}

		listProjectiles.remove_if([](const Object& o) { return o.bRedundant; });
		listEnemies.remove_if([](const Object& o) { return o.bRedundant; });

		auto itRemove = remove_if(listEnemies.begin(), listEnemies.end(), [&](const Object& o) { return o.pos.first <= -nEnemyWidth; });

		if (itRemove != listEnemies.end())
		{
			listEnemies.erase(itRemove);
			AddEnemy();
			nScore--;
		}

		listProjectiles.remove_if([&](const Object& o) { return o.pos.first >= ScreenWidth(); });

		if (nScore < 0)
			bGameOver = true;

		Clear(PIXEL_SOLID, FG_BLACK);

		for (const auto& p : listProjectiles)
			Draw(p.pos.first, p.pos.second, PIXEL_SOLID, FG_DARK_YELLOW);

		auto draw_ship = [&](int sx, int sy, int width, int height, const std::wstring& model)
		{
			for (int x = 0; x < width; x++)
				for (int y = 0; y < height; y++)
					Draw(sx + x, sy + y, model[y * width + x], FG_GREY);
		};

		draw_ship((int)fPlayerX, (int)fPlayerY, nPlayerWidth, nPlayerHeight, sPlayerModel);

		for (const auto& p : listEnemies)
			draw_ship(p.pos.first, p.pos.second, nEnemyWidth, nEnemyHeight, sEnemyModel);

		DrawString(1, 1, L"Score: " + to_wstring(nScore) + L". Pattern: " + to_wstring(nCurrentPattern), FG_WHITE);
		//DrawString(1, 2, L"Projectiles: " + to_wstring(listProjectiles.size()), FG_WHITE);

		return true;
	}

};

int main()
{
	Space demo;
	if (demo.ConstructConsole(120, 40, 16, 16) == RC_OK)
		demo.Run();
	return 0;
}
