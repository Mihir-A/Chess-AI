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
	int getX() const;
	int getY() const;
	Piece* getPiece() const;
	void setPiece(Piece* p);
	void deletePiece();
};

