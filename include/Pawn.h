#pragma once
#include "Piece.h"
class Pawn :
    public Piece
{
private:
    mutable bool firstMove;
public:
    Pawn(bool white, int x, int y);
    bool canMove(int atemptX, int atemptY, const Board& b) const;
};

