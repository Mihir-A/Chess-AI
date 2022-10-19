#pragma once
#include "Piece.h"
class Knight :
    public Piece
{
public:
    Knight(bool white);
    bool canMove(const Spot& o, const Spot& n, const Board& b) const;
};

