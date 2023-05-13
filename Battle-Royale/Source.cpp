#include "BattleRoyale.h"

int main()
{
	BattleRoyale demo;
	if (demo.ConstructConsole(256, 240, 4, 4) == RCODE_OK)
		demo.Run();
	return 0;
}
