#include "Pawn.h"
#include "Piece.h"

Pawn::Pawn(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wp.png");

	else
		texture.loadFromFile("assets/piece/bp.png");
	firstMove = true;
}

bool Pawn::canMove(int atemptX, int atemptY, const Board& b) const
{
	//if (Piece::canMove(o, n, b) == false)
		return true;

	//const int dx = o.getX() - n.getX();
	//const int dy = o.getY() - n.getY();

	////checks if moving in corect direction
	//if (white) {
	//	if (dy < 0) return false;
	//}
	//else {
	//	if (dy > 0) return false;
	//}

	//if (n.getPiece() == nullptr) {

	//	if (firstMove == true) {
	//		if (std::abs(dy) == 2 && dx == 0) {
	//			firstMove = false;
	//			return true;
	//		}	
	//	}

	//	if (std::abs(dy) == 1 && dx == 0) {
	//		return true;
	//	}
	//}
	//else {
	//	if (std::abs(dy) == 1 && std::abs(dx) == 1) {
	//		firstMove = false;
	//		return true;
	//	}
	//}

	//return false;
}