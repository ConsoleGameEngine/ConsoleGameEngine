#include "BattleRoyale.h"

int main()
{
	BattleRoyale game;
	if (game.ConstructConsole(256, 240, 4, 4) == RCODE_OK)
		game.Run();
	return 0;
}
