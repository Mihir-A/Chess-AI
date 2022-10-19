#include "King.h"
#include "Spot.h"
#include <iostream>
King::King(bool white) 
	: Piece(white) 
{
	if (white) 
		texture.loadFromFile("assets/piece/wk.png");
	
	else 
		texture.loadFromFile("assets/piece/bk.png");
}

bool King::canMove(const Spot& o, const Spot& n, const Board& b) const
{
	if (Piece::canMove(o, n, b) == false)
		return false;

	const int dx = std::abs(o.getX() - n.getX());
	const int dy = std::abs(o.getY() - n.getY());

	

	return (dy <= 1 && dx <= 1);
}
