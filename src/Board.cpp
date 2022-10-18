#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "Pawn.h"

Board::Board()
{
	for (int i = 2; i < 6; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			this->b[i][j] = Spot(j, i, nullptr);
		}
	}
	b[0][4] = Spot(4, 0, new King(false));
	b[7][4] = Spot(4, 7, new King(true));

	b[0][3] = Spot(3, 0, new Queen(false));
	b[7][3] = Spot(3, 7, new Queen(true));

	b[0][1] = Spot(1, 0, new Knight(false));
	b[0][6] = Spot(6, 0, new Knight(false));
	b[7][1] = Spot(1, 7, new Knight(true));
	b[7][6] = Spot(6, 7, new Knight(true));

	b[0][2] = Spot(2, 0, new Bishop(false));
	b[0][5] = Spot(5, 0, new Bishop(false));
	b[7][2] = Spot(2, 7, new Bishop(true));
	b[7][5] = Spot(5, 7, new Bishop(true));

	b[0][0] = Spot(0, 0, new Rook(false));
	b[0][7] = Spot(7, 0, new Rook(false));
	b[7][0] = Spot(0, 7, new Rook(true));
	b[7][7] = Spot(7, 7, new Rook(true));

	for (int i = 0; i < 8; i++) {
		b[1][i] = Spot(i, 1, new Pawn(false));
		b[6][i] = Spot(i, 6, new Pawn(true));
	}
}

const Spot& Board::getSpot(int x, int y) const
{
	return b[y][x];
}

void Board::setPiece(int x, int y, Piece* p)
{
	b[y][x].setPiece(p);
}
