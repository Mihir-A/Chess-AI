#pragma once
#include <Board.h>
class Spot;
class Piece
{
protected:
	bool white;
public:
	Piece(bool white);
	bool isWhite();
	virtual bool canMove(Spot o, Spot n, Board b);
};

