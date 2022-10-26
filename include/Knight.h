#pragma once
#include "Piece.h"
class Knight : public Piece
{
public:
    Knight(bool white, int x, int y);
    bool canMove(int atemptX, int atemptY, const Board &b) const;
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const;
};
