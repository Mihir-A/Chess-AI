#include "Knight.h"
#include "Spot.h"
Knight::Knight(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wn.png");

	else
		texture.loadFromFile("assets/piece/bn.png");
}

bool Knight::canMove(const Spot& o, const Spot& n, const Board& b) const
{
	if (Piece::canMove(o, n, b) == false)
		return false;

	const int dx = std::abs(o.getX() - n.getX());
	const int dy = std::abs(o.getY() - n.getY());

	return (dx == 2 && dy == 1 || dx == 1 && dy == 2);
}
