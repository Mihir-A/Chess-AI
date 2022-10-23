#include "Bishop.h"
#include <iostream>
Bishop::Bishop(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wb.png");

	else
		texture.loadFromFile("assets/piece/bb.png");
}

void Bishop::getPossibleMoves(std::vector<Move>& moves, const Board& b) const {

}