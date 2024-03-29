#pragma once
#include "Piece.h"

class Pawn final : public Piece
{
public:
    Pawn(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
};
