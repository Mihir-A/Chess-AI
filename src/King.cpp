#include "King.h"
#include "Board.h"
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

void King::getPossibleMoves(std::vector<Move>& moves, const Board& b) const
{
	const int offsets[3] = {-1, 0, 1};

	for (int xOffset : offsets) {
		for (int yOffset : offsets) {
			if (x + xOffset < 8 && x + xOffset > -1 && y + yOffset < 8 && y + yOffset > -1) {
				if (b.getPiece(x + xOffset, y + yOffset) == nullptr) {
					moves.emplace_back(this, x, y, x + xOffset, y + yOffset);
				}
				else if (b.getPiece(x + xOffset, y + yOffset)->isWhite() != this->isWhite()) {
					moves.emplace_back(this, x, y, x + xOffset, y + yOffset);
				}
			}
		}
	}
}
