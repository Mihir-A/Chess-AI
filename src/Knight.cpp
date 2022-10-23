#include "Knight.h"
Knight::Knight(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wn.png");

	else
		texture.loadFromFile("assets/piece/bn.png");
}

bool Knight::canMove(int atemptX, int atemptY, const Board& b) const
{
	//if (Piece::canMove(o, n, b) == false)
		return true;

//	const int dx = std::abs(o.getX() - n.getX());
	//const int dy = std::abs(o.getY() - n.getY());

//	return (dx == 2 && dy == 1 || dx == 1 && dy == 2);
}

void Knight::getPossibleMoves(std::vector<Move>& moves, const Board& b) const {

}