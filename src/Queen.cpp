#include "Queen.h"
#include "Move.h";
Queen::Queen(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wq.png");

	else
		texture.loadFromFile("assets/piece/bq.png");
}

void Queen::getPossibleMoves(std::vector<Move>& moves, const Board& b) const {

}