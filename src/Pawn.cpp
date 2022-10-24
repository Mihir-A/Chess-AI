#include "Pawn.h"
#include "Move.h"
#include "Board.h"

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

void Pawn::moveTo(int xCord, int yCord) {
	Piece::moveTo(xCord, yCord);
	firstMove = false;
}

void Pawn::getPossibleMoves(std::vector<Move>& moves, const Board& b) const {
	const int yDir = white ? -1 : 1;

	//double pawn push
	if (onBoard(y + yDir * 2)) {
		if (b.getPiece(x, y + yDir * 2) == nullptr && firstMove && b.getPiece(x, y + yDir) == nullptr) {
			moves.emplace_back(this, x, y, x, y + yDir * 2);
		}
	}
	//single pawn push
	if (onBoard(y + yDir)) {
		if (b.getPiece(x, y + yDir) == nullptr) {
			moves.emplace_back(this, x, y, x, y + yDir);
		}
	}
	//pawn right capture
	if (onBoard(y + yDir) && onBoard(x + 1)) {
		if (b.getPiece(x + 1, y + yDir) != nullptr) {
			if(b.getPiece(x + 1, y + yDir)->isWhite() != this->isWhite())
				moves.emplace_back(this, x, y, x + 1, y + yDir);
		}
	}
	//pawn left capture
	if (onBoard(y + yDir) && onBoard(x - 1)) {
		if (b.getPiece(x - 1, y + yDir) != nullptr) {
			if (b.getPiece(x - 1, y + yDir)->isWhite() != this->isWhite())
				moves.emplace_back(this, x, y, x - 1, y + yDir);
		}
	}
}