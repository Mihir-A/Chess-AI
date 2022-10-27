#pragma once
#include "Piece.h"
class Pawn : public Piece
{
private:
    bool firstMove;

public:
    Pawn(bool white, int x, int y);
    void moveTo(int xCord, int yCord) override;
    void getPossibleMoves(std::vector<Move> &moves, const Board &b) const override;
};
