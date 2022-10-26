#pragma once
#include "Piece.h"
class Bishop : public Piece
{
public:
    Bishop(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const;
};
