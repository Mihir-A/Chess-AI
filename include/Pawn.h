#pragma once
#include "Piece.h"
class Pawn :
    public Piece
{
private:
    mutable bool firstMove;
public:
    Pawn(bool white);
    bool canMove(const Spot& o, const Spot& n, const Board& b) const;
};

