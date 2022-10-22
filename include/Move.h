#pragma once
#include "Piece.h"
class Move
{
private:
	const Piece *p;
	int oldX, oldY, newX, newY;
public:
	Move(const Piece* p, int oldX, int oldY, int newX, int newY);
	const Piece* getPiece();
	int getOldX();
	int getOldY();
	int getNewX();
	int getNewY();
};

