#include "ConsoleGameEngine.h"

#include <list>

class AStar : public def::ConsoleGameEngine
{
public:
	AStar()
	{
		sAppName = L"AStar algorithm";
	}

private:
	struct Node
	{
		bool bObstacle = false;
		bool bVisited = false;
		float fGlobalGoal;
		float fLocalGoal;
		int x;
		int y;
		std::vector<Node*> vecNeighbours;
		Node* parent;
	};

	Node* nodes = nullptr;

	Node* start;
	Node* goal;
	
	int nMapWidth = 16;
	int nMapHeight = 16;

protected:
	void DoAStar()
	{
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				int p = y * nMapWidth + x;
				nodes[p].bVisited = false;
				nodes[p].fGlobalGoal = INFINITY;
				nodes[p].fLocalGoal = INFINITY;
				nodes[p].parent = nullptr;
			}

		auto dist = [](Node* n1, Node* n2)
		{
			return sqrtf((n1->x - n2->x) * (n1->x - n2->x) + (n1->y - n2->y) * (n1->y - n2->y));
		};

		auto heuristic = [dist](Node* n1, Node* n2)
		{
			return dist(n1, n2);
		};

		Node* current = start;
		start->fLocalGoal = 0.0f;
		start->fGlobalGoal = heuristic(start, goal);

		std::list<Node*> listNodesToTest;
		listNodesToTest.push_back(start);
		
		while (!listNodesToTest.empty() && current != goal)
		{
			listNodesToTest.sort(
				[](const Node* lhs, const Node* rhs)
				{
					return lhs->fGlobalGoal < rhs->fGlobalGoal;
				}
			);

			while (!listNodesToTest.empty() && listNodesToTest.front()->bVisited)
				listNodesToTest.pop_front();

			if (listNodesToTest.empty())
				break;

			current = listNodesToTest.front();
			current->bVisited = true;

			for (auto n : current->vecNeighbours)
			{
				if (!n->bVisited && !n->bObstacle)
					listNodesToTest.push_back(n);

				float fPossiblyLowerGoal = current->fLocalGoal + dist(current, n);

				if (fPossiblyLowerGoal < n->fLocalGoal)
				{
					n->parent = current;
					n->fLocalGoal = fPossiblyLowerGoal;
					n->fGlobalGoal = n->fLocalGoal + heuristic(n, goal);
				}
			}
		}
	}

	bool OnUserCreate() override
	{
		nodes = new Node[nMapWidth * nMapHeight];
		
		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				int p = y * nMapWidth + x;
				nodes[p].bObstacle = false;
				nodes[p].bVisited = false;
				nodes[p].x = x;
				nodes[p].y = y;
				nodes[p].parent = nullptr;
			}

		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				if (y > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y - 1) * nMapWidth + x]);

				if (y < nMapHeight - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[(y + 1) * nMapWidth + x]);

				if (x > 0)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + x - 1]);

				if (x < nMapWidth - 1)
					nodes[y * nMapWidth + x].vecNeighbours.push_back(&nodes[y * nMapWidth + x + 1]);
			}

		start = &nodes[(nMapHeight / 2) * nMapWidth + 1];
		goal = &nodes[(nMapHeight / 2) * nMapWidth + nMapWidth - 2];

		DoAStar();

		return true;
	}

	bool OnUserUpdate(float fDeltaTime) override
	{
		int nNodeSize = 9;
		int nNodeBorder = 2;

		int nSelectedNodeX = GetMouseX() / nNodeSize;
		int nSelectedNodeY = GetMouseY() / nNodeSize;

		if (GetMouseBtn(0).bReleased)
		{
			if (nSelectedNodeX >= 0 && nSelectedNodeX < nMapWidth)
				if (nSelectedNodeY >= 0 && nSelectedNodeY < nMapHeight)
				{
					int p = nSelectedNodeY * nMapWidth + nSelectedNodeX;

					if (GetKey(L'S').bHeld)
						start = &nodes[p];
					else if (GetKey(L'G').bHeld)
						goal = &nodes[p];
					else
					{
						if (&nodes[p] != start && &nodes[p] != goal)
							nodes[p].bObstacle = !nodes[p].bObstacle;
					}

					DoAStar();
				}
		}

		Clear(def::PIXEL_SOLID, def::FG_BLACK);

		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				for (auto n : nodes[y * nMapWidth + x].vecNeighbours)
					DrawLine(x * nNodeSize + nNodeSize / 2, y * nNodeSize + nNodeSize / 2, n->x * nNodeSize + nNodeSize / 2, n->y * nNodeSize + nNodeSize / 2, def::Pixel::SOLID, def::FG::DARK_BLUE);
			}

		for (int x = 0; x < nMapWidth; x++)
			for (int y = 0; y < nMapHeight; y++)
			{
				int x1 = x * nNodeSize + nNodeBorder;
				int y1 = y * nNodeSize + nNodeBorder;
				int x2 = (x + 1) * nNodeSize - nNodeBorder;
				int y2 = (y + 1) * nNodeSize - nNodeBorder;

				if (nodes[y * nMapWidth + x].bObstacle)
					FillRectangle(x1, y1, x2, y2, def::PIXEL_SOLID, def::FG_WHITE);
				else if (&nodes[y * nMapWidth + x] == start)
					FillRectangle(x1, y1, x2, y2, def::PIXEL_SOLID, def::FG_GREEN);
				else if (&nodes[y * nMapWidth + x] == goal)
					FillRectangle(x1, y1, x2, y2, def::PIXEL_SOLID, def::FG_RED);
				else if (nodes[y * nMapWidth + x].bVisited)
					FillRectangle(x1, y1, x2, y2, def::PIXEL_SOLID, def::FG_BLUE);
				else
					FillRectangle(x1, y1, x2, y2, def::PIXEL_SOLID, def::FG_DARK_BLUE);
			}

		if (goal != nullptr)
		{
			Node* p = goal;
			while (p->parent != nullptr)
			{
				DrawLine(p->x * nNodeSize + nNodeSize / 2, p->y * nNodeSize + nNodeSize / 2, p->parent->x * nNodeSize + nNodeSize / 2, p->parent->y * nNodeSize + nNodeSize / 2, def::PIXEL_SOLID, def::FG_YELLOW);

				p = p->parent;
			}
		}

		return true;
	}

};

int main()
{
	AStar demo;
	def::rcode err = demo.ConstructConsole(256, 240, 4, 4);

	if (err.ok)
		demo.Run();
	else
		std::cerr << err.info << "\n";

	return 0;
}
