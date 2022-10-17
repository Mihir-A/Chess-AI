#include "Board.h"
#include "King.h"

Board::Board()
{
	for (int i = 2; i < 5; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			this->b[i][j] = Spot(i, j, nullptr);
		}
	}
	b[0][0] = Spot(0, 0, new King(true));
}

const Spot& Board::getSpot(int x, int y) const
{
	return b[x][y];
}
