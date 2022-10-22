#pragma once
#include "Piece.h"
class King :
    public Piece
{
public:
    King(bool white, int x, int y);
    bool canMove(int atemptX, int atemptY, const Board& b) const;
};

