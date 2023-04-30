#include "ConsoleGameEngine.h"

#undef max
#undef min

using namespace std;

struct sSpaceObject
{
	float x;
	float y;

	float dx;
	float dy;

	int size;

	float angle;
};

class Asteroids : public ConsoleGameEngine
{
public:
	Asteroids()
	{
		sAppName = L"Asteroids";
	}

protected:
	bool OnUserCreate() override
	{
		ResetGame();

		vecPlayerVerticies.push_back({ 0.0f, -5.5f });
		vecPlayerVerticies.push_back({ -2.5f, 2.5f });
		vecPlayerVerticies.push_back({ 2.5f, 2.5f });

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		if (bDead) ResetGame();

		float fContactPointX = 0.0f;
		float fContactPointY = 0.0f;

		float fContactNormalX = 0.0f;
		float fContactNormalY = 0.0f;

		float fRayOriginX = 0.0f;
		float fRayOriginY = 0.0f;

		for (auto& a : vecAsteroids)
		{
			float t = 0.0f;

			fRayOriginX = player.x + vecPlayerVerticies[0].first;
			fRayOriginY = player.y + vecPlayerVerticies[0].second;

			bool bFirst = RayVsCircle(fRayOriginX, fRayOriginY, player.x + vecPlayerVerticies[1].first, player.y + vecPlayerVerticies[1].second, &a, fContactPointX, fContactPointY, fContactNormalX, fContactNormalY, t) && t < 0.15f;

			fRayOriginX = player.x + vecPlayerVerticies[1].first;
			fRayOriginY = player.y + vecPlayerVerticies[1].second;

			bool bSecond = RayVsCircle(fRayOriginX, fRayOriginY, player.x + vecPlayerVerticies[2].first, player.y + vecPlayerVerticies[2].second, &a, fContactPointX, fContactPointY, fContactNormalX, fContactNormalY, t) && t < 0.15f;

			fRayOriginX = player.x + vecPlayerVerticies[2].first;
			fRayOriginY = player.y + vecPlayerVerticies[2].second;

			bool bThird = RayVsCircle(fRayOriginX, fRayOriginY, player.x + vecPlayerVerticies[0].first, player.y + vecPlayerVerticies[0].second, &a, fContactPointX, fContactPointY, fContactNormalX, fContactNormalY, t) && t < 0.15f;

			if (bFirst || bSecond || bThird)
				bDead = true;
		}

		if (GetKey(VK_LEFT).bHeld)
			player.angle -= 5.0f * fDeltaTime;

		if (GetKey(VK_RIGHT).bHeld)
			player.angle += 5.0f * fDeltaTime;

		if (GetKey(VK_UP).bHeld)
		{
			player.dx += sin(player.angle) * 20.0f * fDeltaTime;
			player.dy -= cos(player.angle) * 20.0f * fDeltaTime;
		}

		player.x += player.dx * fDeltaTime;
		player.y += player.dy * fDeltaTime;

		WrapCoords(player.x, player.y, player.x, player.y);

		if (GetMouse(0).bPressed)
			vecBullets.push_back({ player.x, player.y, 50.0f * sin(player.angle), -50.0f * cos(player.angle), 0, 0.0f });

		if (vecBullets.size() > 0)
		{
			auto it = remove_if(vecBullets.begin(), vecBullets.end(),
				[&](sSpaceObject& o)
				{
					return o.x <= 0 || o.y <= 0 || o.x > ScreenWidth() || o.y > ScreenHeight();
				});

			if (it != vecBullets.end())
				vecBullets.erase(it);
		}

		for (int j = 0; j < vecBullets.size(); j++)
		{
			vecBullets[j].x += vecBullets[j].dx * 6.0f * fDeltaTime;
			vecBullets[j].y += vecBullets[j].dy * 6.0f * fDeltaTime;

			WrapCoords(vecBullets[j].x, vecBullets[j].y, vecBullets[j].x, vecBullets[j].y);

			for (int i = 0; i < vecAsteroids.size(); i++)
			{
				if (PointVsCircle(vecAsteroids[i].x, vecAsteroids[i].y, vecAsteroids[i].size, vecBullets[j].x, vecBullets[j].y))
				{
					const sSpaceObject& o = vecAsteroids[i];

					vecAsteroids.erase(vecAsteroids.begin() + i);

					if (o.size > 1)
					{
						vecAsteroids.push_back({
								o.x, o.y, 8.0f, -6.0f, o.size / 2, 0.0f
							});

						vecAsteroids.push_back({
								o.x, o.y, -8.0f, 6.0f, o.size / 2, 0.0f
							});
					}
					else
						vecAsteroids.push_back({ (float)(rand() % 100 + 30), (float)(rand() % 100 + 30), -8.0f, 6.0f, rand() % 10, 0.0f });

					vecBullets.erase(vecBullets.begin() + j);

					nScore++;

					break;
				}
			}
		}

		Clear(PIXEL_SOLID, FG_BLACK);

		for (auto& a : vecAsteroids)
		{
			a.x += a.dx * 5.0f * fDeltaTime;
			a.y += a.dy * 5.0f * fDeltaTime;

			WrapCoords(a.x, a.y, a.x, a.y);

			DrawCircle(a.x, a.y, a.size, PIXEL_SOLID, FG_YELLOW);
		}

		DrawWireFrameModel(vecPlayerVerticies, player.x, player.y, player.angle, 2.0f);

		DrawString(10, 10, L"Score: " + to_wstring(nScore), FG_WHITE);

		for (const auto& bullet : vecBullets)
			Draw(bullet.x, bullet.y);

		return true;
	}

	void ResetGame()
	{
		vecAsteroids.clear();
		vecBullets.clear();

		vecAsteroids.push_back({
				20.0f, 20.0f, 8.0f, -6.0f, 16, 0.0f
			});

		player.x = (float)ScreenWidth() / 2.0f;
		player.y = (float)ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		bDead = false;
		nScore = 0;
	}

	void WrapCoords(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;

		if (ix < 0.0f) ox += (float)ScreenWidth();
		if (iy < 0.0f) oy += (float)ScreenHeight();

		if (ix > (float)ScreenWidth()) ox -= (float)ScreenWidth();
		if (iy > (float)ScreenHeight()) oy -= (float)ScreenHeight();
	}

	bool PointVsCircle(float cx, float cy, float r, float x, float y)
	{
		return sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) < r;
	}

	bool RayVsCircle(float& fRayOriginX, float& fRayOriginY, float fRayDirX, float fRayDirY, const sSpaceObject* target, float fContactX, float fContactY, float fContactNormalX, float fContactNormalY, float& fTHitNear)
	{
		fContactNormalX = 0;
		fContactNormalY = 0;

		fContactX = 0;
		fContactY = 0;

		float fInvDirX = 1.0f / fRayDirX;
		float fInvDirY = 1.0f / fRayDirY;

		float fTNearX = (target->x - fRayOriginX) * fInvDirX;
		float fTNearY = (target->y - fRayOriginY) * fInvDirY;

		float fTFarX = (target->x + target->size - fRayOriginX) * fInvDirX;
		float fTFarY = (target->y + target->size - fRayOriginY) * fInvDirY;

		if (isnan(fTFarY) || isnan(fTFarX)) return false;
		if (isnan(fTNearY) || isnan(fTNearX)) return false;

		if (fTNearX > fTFarX) swap(fTNearX, fTFarX);
		if (fTNearY > fTFarY) swap(fTNearY, fTFarY);

		if (fTNearX > fTFarY || fTNearY > fTFarX) return false;

		fTHitNear = max(fTNearX, fTNearY);

		float fTHitFar = min(fTFarX, fTFarY);

		if (fTHitFar < 0.0f)
			return false;

		fContactX = fRayOriginX + fTHitNear * fRayDirX;
		fContactY = fRayOriginY + fTHitNear * fRayDirY;

		if (fTNearX > fTNearY)
			if (fInvDirX < 0.0f)
			{
				fContactNormalX = 1.0f;
				fContactNormalY = 0.0f;
			}
			else
			{
				fContactNormalX = -1.0f;
				fContactNormalY = 0.0f;
			}
		else if (fTNearX < fTNearY)
			if (fInvDirY < 0.0f)
			{
				fContactNormalX = 0.0f;
				fContactNormalY = 1.0f;
			}
			else
			{
				fContactNormalX = 0.0f;
				fContactNormalY = -1.0f;
			}

		return true;
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F) override
	{
		float fx, fy;
		WrapCoords(x, y, fx, fy);
		ConsoleGameEngine::Draw((int)fx, (int)fy, c, col);
	}

private:
	vector<sSpaceObject> vecAsteroids;
	vector<sSpaceObject> vecBullets;

	sSpaceObject player;

	vector<pair<float, float>> vecPlayerVerticies;

	int nScore = 0;
	bool bDead = false;

};

int main()
{
	Asteroids demo;
	if (demo.ConstructConsole(160, 100, 8, 8) == RCODE_OK)
		demo.Run();
	return 0;
}
