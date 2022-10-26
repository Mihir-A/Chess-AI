#include "Knight.h"
#include "Move.h"
#include "Board.h"

Knight::Knight(bool white, int x, int y)
    : Piece(white, x, y, "Knight")
{
	if (white)
		texture.loadFromFile("assets/piece/wn.png");

	else
		texture.loadFromFile("assets/piece/bn.png");
}

void Knight::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{
	// Tall knight moves

	for (const int dx : {-2, 2})
	{
		for (const int dy : {-1, 1})
		{
			if (onBoard(x + dx) && onBoard(y + dy))
			{
				if (b.getPiece(x + dx, y + dy) == nullptr)
				{
					moves.emplace_back(this, x, y, x + dx, y + dy);
				}
				// Checks if color of target != to the piece color
				else if (b.getPiece(x + dx, y + dy)->isWhite() != white)
				{
					moves.emplace_back(this, x, y, x + dx, y + dy);
				}
			}
		}
	}

	// Wide knight moves
	for (const int dx : {-1, 1})
	{
		for (const int dy : {-2, 2})
		{
			if (onBoard(x + dx) && onBoard(y + dy))
			{
				if (b.getPiece(x + dx, y + dy) == nullptr)
				{
					moves.emplace_back(this, x, y, x + dx, y + dy);
				}
				// Checks if color of target != to the piece color
				else if (b.getPiece(x + dx, y + dy)->isWhite() != white)
				{
					moves.emplace_back(this, x, y, x + dx, y + dy);
				}
			}
		}
	}
}