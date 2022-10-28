#pragma once
#include "Piece.h"

class Knight final : public Piece
{
public:
    Knight(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
};
