#include "Pawn.h"
#include "Piece.h"
#include "Spot.h"

Pawn::Pawn(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wp.png");

	else
		texture.loadFromFile("assets/piece/bp.png");
	firstMove = true;
}

bool Pawn::canMove(const Spot& o, const Spot& n, const Board& b) const
{
	if (Piece::canMove(o, n, b) == false)
		return false;

	const int dx = o.getX() - n.getX();
	const int dy = o.getY() - n.getY();

	if (n.getPiece() == nullptr) {
		if (firstMove == true) {
			firstMove = false;
			
		}
	}
	else {

	}
	return (dy <= 1 && dx <= 1);
}