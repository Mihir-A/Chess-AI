#pragma once
#include "Piece.h"

class King : public Piece
{
public:
    King(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
    bool inCheck(Board b);
};
