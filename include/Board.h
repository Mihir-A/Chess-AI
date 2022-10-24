#pragma once
#include <vector>
#include <array>
#include "Piece.h"
class Move;
class Board
{
private:
	std::array<std::array<Piece*, 8>, 8> b;
public:
	Board();
	const Piece* getPiece(int x, int y) const;
	void setPiece(int newX, int newY, int origX, int origY);
	void setPieceNull(int x, int y);
	void makeMove(const Move& m);
};

