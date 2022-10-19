#pragma once
#include "Piece.h"
class King :
    public Piece
{
public:
    King(bool white);
    bool canMove(const Spot& o, const Spot& n, const Board& b) const;
};

