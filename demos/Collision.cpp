#include "defConsoleGameEngine.h"

class Collision : public def::ConsoleGameEngine
{
public:
	Collision()
	{
		sAppName = L"Collision Detection";
	}

protected:
	virtual bool OnUserCreate() override
	{
		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		auto IsOverlap = [](int x1, int y1, int x2, int y2, int i1, int j1, int i2, int j2)
		{
			if (((x2 == i2 || x1 == i1) &&
				(j1 >= y2 && j2 <= y2)) ||
				((i2 >= x1 && i1 <= x2) &&
				(y1 == j2)            )	||
				((x1 == i1            )	&&
				(j1 >= y1 && j2 <= y2)) ||
				((y2 == j1			  )	&&
				(i2 >= x1 && i1 <= x2)))
				return true;
			return false;
		};

		Clear(def::FG::DARK_BLUE);
		if (mouse[0].bPressed)
			rectPos.push_back(GetMouse());

		if (mouse[1].bPressed)
			vToDelete = GetMouse();

		vOldPlayerPos = vPlayerPos;

		if (keys[VK_LEFT].bHeld)
		{
			vPlayerPos.x -= 1.0f;
		}

		if (keys[VK_RIGHT].bHeld)
		{
			vPlayerPos.x += 1.0f;
		}

		if (keys[VK_UP].bHeld)
		{
			vPlayerPos.y -= 1.0f;
		}

		if (keys[VK_DOWN].bHeld)
		{
			vPlayerPos.y += 1.0f;
		}

		if (keys[VK_SPACE].bPressed)
			for (auto& p : rectPos)
				rectPos.clear();

		DrawRectangle(vPlayerPos, { vPlayerPos.x + 5, vPlayerPos.y + 5 }, def::Pixel::SOLID, def::FG::RED);

		int index = 0;

		for (auto& p : rectPos)
		{
			if ((vToDelete.x >= p.x) && (vToDelete.y >= p.y) && (vToDelete.x <= p.x + 10) && (vToDelete.y <= p.y + 10))
				rectPos.erase(rectPos.begin() + index);

			DrawRectangle(p, { p.x + 10, p.y + 10 }, def::Pixel::SOLID, def::FG::WHITE);
			DrawLine(p.x, p.y, p.x + 10, p.y + 10, def::Pixel::SOLID, def::FG::WHITE);
			DrawLine(p.x + 10, p.y, p.x, p.y + 10, def::Pixel::SOLID, def::FG::WHITE);

			if (IsOverlap(p.x + 5, p.y + 5, p.x + 10, p.y + 10, vPlayerPos.x, vPlayerPos.y, vPlayerPos.x + 10, vPlayerPos.y + 10) ||
				IsOverlap(p.y + 5, p.x + 5, p.y + 10, p.x + 10, vPlayerPos.y, vPlayerPos.x, vPlayerPos.y + 10, vPlayerPos.x + 10))
				vPlayerPos = vOldPlayerPos;

			index++;
		}

		return true;
	}

private:
	std::vector<def::vi2d> rectPos;
	def::vi2d vPlayerPos = { 0, 0 };
	def::vi2d vOldPlayerPos = { 0, 0 };
	def::vi2d vToDelete;

};

int main()
{
	Collision demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	else
		std::wcout << L"Couldn't construct console" << std::endl;

	return 0;
}
