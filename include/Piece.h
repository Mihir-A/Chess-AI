#pragma once
class Piece
{
protected:
	bool white;
public:
	Piece(bool white);
	bool isWhite();
	virtual bool canMove() = 0;
};

