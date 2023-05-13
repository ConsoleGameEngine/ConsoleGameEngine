#include "BattleRoyale.h"

bool PointVsCircle(float px, float py, float cx, float cy, float r)
{
	float x = px - cx;
	float y = py - cy;
	return x * x + y * y < r* r;
}

BattleRoyale::BattleRoyale()
{
	sAppName = L"Battle-Royale";

	sprFont = nullptr;
	pControl = nullptr;

	nGameState = GS_CREDITS;
}

BattleRoyale::~BattleRoyale()
{
	delete sprFont;
}

void BattleRoyale::CreateShip(float x, float y, wstring name)
{
	if (listShips.size() < MAX_PLAYERS)
	{
		name = name.substr(0, MAX_NAME_LENGTH);

		sShip s;

		s.x = x;	 s.y = y;
		s.dx = 0.0f; s.dy = 0.0f;
		s.heading = 0.0f;

		s.health = MAX_SHIP_HEALTH;
		s.name = name;
		s.score = 0;

		s.id = GenerateId();
		s.col = s.id + 1;

		listShips.push_back(s);
	}
}

void BattleRoyale::CreateBullet(float x, float y, float dx, float dy, short col)
{
	sBullet b;

	b.x = x; b.y = y;
	b.dx = dx; b.dy = dy;
	b.lifeTime = BULLET_LIFETIME;

	b.col = col;

	listBullets.push_back(b);
}

int BattleRoyale::GenerateId()
{
	int nId = 0;

	while (1)
	{
		size_t i = 0;
		for (const auto& s : listShips)
		{
			if (s.id == nId)
			{
				nId = rand() % 13 + 1;
				break;
			}

			i++;
		}

		if (i == listShips.size())
			break;
	}

	return nId;
}

void BattleRoyale::DrawBigText(int x, int y, const wstring& text, short col)
{
	int sx = 0, sy = 0;
	for (auto wc : text)
	{
		char c = (char)wc;

		if (c == '\n')
		{
			sx = 0;
			sy += 8;
		}
		else if (c == '\t')
			sx += 32;
		else
		{
			int ox = (c - 32) % 16;
			int oy = (c - 32) / 16;

			for (int i = 0; i < 8; i++)
				for (int j = 0; j < 8; j++)
					if (sprFont->GetColour(i + ox * 8, j + oy * 8) > 0)
						Draw(x + sx + i, y + sy + j, PIXEL_SOLID, col);

			sx += 8;
		}
	}
}

bool BattleRoyale::LoadPlayersData(const wstring& sFileName)
{
	wifstream file(sFileName);
	if (!file.is_open()) return false;

	while (!file.eof())
	{
		wstring s;
		getline(file, s);

		CreateShip(
			stoi(s.substr(s.find_first_of(L',') + 1, s.find_last_of(L','))),
			stoi(s.substr(s.find_last_of(L',') + 2)),
			s.substr(1, s.find_first_of(L',') - 2)
		);
	}

	file.close();

	return true;
}

void BattleRoyale::ConstructFont()
{
	string data =
		"?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000"
		"O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400"
		"Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000"
		"720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000"
		"OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000"
		"ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000"
		"Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000"
		"70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000"
		"OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000"
		"00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000"
		"<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000"
		"O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000"
		"00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000"
		"Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0"
		"O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000"
		"?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

	sprFont = new Sprite(128, 48);
	int px = 0, py = 0;

	for (int b = 0; b < 1024; b += 4)
	{
		unsigned int sym1 = (unsigned int)data[b + 0] - 48;
		unsigned int sym2 = (unsigned int)data[b + 1] - 48;
		unsigned int sym3 = (unsigned int)data[b + 2] - 48;
		unsigned int sym4 = (unsigned int)data[b + 3] - 48;
		unsigned int r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

		for (int i = 0; i < 24; i++)
		{
			short k = (r & (1 << i)) ? 15 : 0;
			sprFont->SetColour(px, py, k);
			if (++py == 48) { px++; py = 0; }
		}
	}
}

bool BattleRoyale::OnUserCreate()
{
	ConstructFont();

	if (LoadPlayersData(L"players.dat"))
	{
		if (listShips.size() > 0)
			pControl = &listShips.back();

		return true;
	}

	return false;
}

bool BattleRoyale::OnUserUpdate(float fDeltaTime)
{
	Clear(PIXEL_SOLID, FG_WHITE);

	if (GetKey(VK_ESCAPE).bPressed) nGameState = !nGameState;

	switch (nGameState)
	{
	case GS_GAME:	 return UpdateGameScreen(fDeltaTime);
	case GS_CREDITS: return UpdateCreditsScreen();
	}
}

void BattleRoyale::WrapCoords(float& x, float& y)
{
	if (x < 0.0f) x += (float)ScreenWidth();
	if (y < 0.0f) y += (float)ScreenHeight();

	if (x >= (float)ScreenWidth())  x -= (float)ScreenWidth();
	if (y >= (float)ScreenHeight()) y -= (float)ScreenHeight();
}

void BattleRoyale::HandleMoves(float fDeltaTime)
{
	if (GetMouse(1).bPressed)
	{
		for (auto& s : listShips)
		{
			if (PointVsCircle(MouseX(), MouseY(), s.x, s.y, 3.5f))
			{
				pControl = &s;
				break;
			}
		}
	}

	if (pControl != nullptr)
	{
		if (GetMouse(0).bPressed)
		{
			CreateBullet(
				pControl->x, pControl->y,
				sin(pControl->heading) * 80.0f,
				-cos(pControl->heading) * 80.0f,
				pControl->col
			);
		}

		if (GetKey(VK_LEFT).bHeld) pControl->heading -= fDeltaTime;
		if (GetKey(VK_RIGHT).bHeld) pControl->heading += fDeltaTime;

		int nDir = 0;
		if (GetKey(VK_UP).bHeld)	nDir = 1;
		if (GetKey(VK_DOWN).bHeld)	nDir = -1;

		pControl->dx += nDir * sin(pControl->heading) * 30.0f * fDeltaTime;
		pControl->dy += nDir * -cos(pControl->heading) * 30.0f * fDeltaTime;
	}
}

void BattleRoyale::HandleCollision()
{
	for (auto& s : listShips)
	{
		for (auto& b : listBullets)
		{
			if (s.col != b.col && !b.redundant && !s.redundant)
			{
				if (PointVsCircle(b.x, b.y, s.x, s.y, 3.5f))
				{
					b.redundant = true;
					s.redundant = (--s.health <= 0);

					if (s.redundant)
					{
						int nAuthorId = b.col - 1;
						for (auto& a : listShips)
						{
							if (nAuthorId == a.id)
								a.score++;
						}
					}
				}
			}
		}
	}
}

void BattleRoyale::CleanUp()
{
	listBullets.remove_if([](sBullet& b) { return b.redundant || b.lifeTime <= 0.0f; });
	listShips.remove_if([](sShip& s) { return s.redundant; });

	listShips.sort([](const sShip& s1, const sShip& s2) { return s2.score < s1.score; });
}

void BattleRoyale::DrawShips(float fDeltaTime)
{
	for (auto& s : listShips)
	{
		s.x += s.dx * fDeltaTime;
		s.y += s.dy * fDeltaTime;
		WrapCoords(s.x, s.y);

		DrawWireFrameModel(
			vecShipVertices,
			s.x, s.y,
			s.heading, 1.0f,
			PIXEL_SOLID, s.col
		);
	}

	if (pControl != nullptr)
		DrawCircle(pControl->x, pControl->y, 6, PIXEL_SOLID, FG_YELLOW);
}

void BattleRoyale::DrawBullets(float fDeltaTime)
{
	for (auto& b : listBullets)
	{
		b.x += b.dx * fDeltaTime;
		b.y += b.dy * fDeltaTime;
		WrapCoords(b.x, b.y);

		b.lifeTime -= fDeltaTime;

		Draw(b.x, b.y, PIXEL_SOLID, b.col);
	}
}

void BattleRoyale::DrawScoreTable()
{
	int x = ScreenWidth() - 96;
	int y = 0;
	for (const auto& s : listShips)
	{
		FillRectangle(x, y, 96, 14, PIXEL_SOLID, FG_BLACK);

		wstring sInfo = to_wstring(s.score) + L' ' + s.name;

		DrawBigText(x + 2, y + 2, sInfo, FG_WHITE | BG_WHITE);
		FillRectangle(
			x + 2, y + 10,
			92 * ((float)s.health / (float)MAX_SHIP_HEALTH), 2,
			PIXEL_SOLID, s.col
		);

		y += 15;
	}
}

bool BattleRoyale::UpdateGameScreen(float fDeltaTime)
{
	HandleMoves(fDeltaTime);
	HandleCollision();
	CleanUp();

	DrawShips(fDeltaTime);
	DrawBullets(fDeltaTime);
	DrawScoreTable();

	return true;
}

bool BattleRoyale::UpdateCreditsScreen()
{
	DrawBigText(70, 80, L" Battle-Royal!", FG_BLUE);
	DrawBigText(70, 90, L"Made by def1ni7", FG_BLUE);

	return true;
}
