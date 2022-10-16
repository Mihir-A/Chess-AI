#pragma once
#include "Piece.h"
class Spot
{
private:
	Piece* p;
	int x, y;
public:
	Spot();
	Spot(int x, int y, Piece* p);
	int getX() const;
	int getY() const;
	Piece* getPiece() const;

};

