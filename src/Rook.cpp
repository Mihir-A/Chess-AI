#include "Rook.h"
#include "Move.h"
#include <iostream>
Rook::Rook(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wr.png");

	else
		texture.loadFromFile("assets/piece/br.png");
}

void Rook::getPossibleMoves(std::vector<Move>& moves, const Board& b) const {

}