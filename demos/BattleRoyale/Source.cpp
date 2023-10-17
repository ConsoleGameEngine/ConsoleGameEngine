#include "BattleRoyale.hpp"

int main()
{
	BattleRoyale game;
	if (game.ConstructConsole(256, 240, 4, 4) == rcode::OK)
		game.Run();
	return 0;
}
