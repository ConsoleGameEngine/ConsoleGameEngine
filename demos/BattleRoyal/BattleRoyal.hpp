#pragma once

#include "ConsoleGameEngine.hpp"

#include <algorithm>
#include <list>
#include <fstream>

#undef min
#undef max

using namespace std;

constexpr int MAX_SHIP_HEALTH = 10;
constexpr float BULLET_LIFETIME = 2.0f;

constexpr int MAX_PLAYERS = 13;
constexpr int MAX_NAME_LENGTH = 8;

const vector<pair<float, float>> vecShipVertices =
{
	{  0.0f, -5.5f },
	{ -2.5f,  2.5f },
	{  2.5f,  2.5f }
};

struct sShip
{
	float fPosX, fPosY;
	float fDirX, fDirY;
	float fHeading;

	int nHealth;
	int nScore;
	wstring sName;

	int nId;
	short nCol;

	bool bRedundant = false;
};

struct sBullet
{
	float fPosX, fPosY;
	float fDirX, fDirY;

	float fLifeTime;
	short nCol;

	bool bRedundant = false;
};

enum GAME_STATE : int
{
	GS_GAME,
	GS_CREDITS
};

bool PointVsCircle(float px, float py, float cx, float cy, float r);

class BattleRoyale : public ConsoleGameEngine
{
public:
	BattleRoyale();
	virtual ~BattleRoyale();

private:
	list<sShip> listShips;
	list<sBullet> listBullets;

	sShip* pControl;
	Sprite* sprFont;

	int nGameState;

private:
	void CreateShip(float fPosX, float fPosY, wstring sName);
	void CreateBullet(float fPosX, float fPosY, float fDirX, float fDirY, short nCol);

	int GenerateId();

	void DrawBigText(int x, int y, const wstring& text, short col);

	bool LoadPlayersData(const wstring& sFileName);
	void ConstructFont();

protected:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fDeltaTime) override;

	void WrapCoords(float& x, float& y);

	void HandleMoves(float fDeltaTime);
	void HandleCollision();

	void CleanUp();
	void DrawShips(float fDeltaTime);
	void DrawBullets(float fDeltaTime);
	void DrawScoreTable();

	bool UpdateGameScreen(float fDeltaTime);
	bool UpdateCreditsScreen();

};
