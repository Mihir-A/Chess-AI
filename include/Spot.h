#pragma once
class Piece;
class Spot
{
private:
	Piece* p;
	int x, y;
public:
	Spot();
	Spot(int x, int y, Piece* p);
	int getX();
	int getY();
	Piece* getPiece();
};

