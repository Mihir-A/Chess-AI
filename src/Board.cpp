#include "Board.h"

Board::Board()
{
	for (int i = 2; i < 5; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			this->b[i][j] = Spot(i, j, nullptr);
		}
	}
}
