#pragma once
#include "Game.h"
class Piece
{
protected:
	bool white;
public:
	Piece(bool white);
	bool isWhite();
	virtual bool canMove(const Spot& s) = 0;
};

