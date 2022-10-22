#include "King.h"
#include <iostream>
King::King(bool white, int x, int y) 
	: Piece(white, x, y) 
{
	if (white) 
		texture.loadFromFile("assets/piece/wk.png");
	
	else 
		texture.loadFromFile("assets/piece/bk.png");
}

bool King::canMove(int atemptX, int atemptY, const Board& b) const
{
	//if (Piece::canMove(atemptX, atemptY, b) == false)
		return true;

	//const int dx = std::abs(getX() - n.getX());
	//const int dy = std::abs(getY() - n.getY());

	

	
}
