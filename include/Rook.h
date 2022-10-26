#pragma once
#include "Piece.h"
class Rook : public Piece
{
public:
    Rook(bool white, int x, int y);
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const;
};
