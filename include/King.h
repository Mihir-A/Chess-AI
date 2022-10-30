#pragma once
#include "Piece.h"

class King final : public Piece
{
public:
    King(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
    bool inCheck(const Board &b) const;
};
