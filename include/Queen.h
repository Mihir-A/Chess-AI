#pragma once
#include "Piece.h"

class Queen final : public Piece
{
public:
    Queen(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
};
