#pragma once
#include "Piece.h"

class Rook final : public Piece
{
public:
    Rook(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
};
