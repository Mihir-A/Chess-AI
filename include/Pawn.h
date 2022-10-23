#pragma once
#include "Piece.h"
class Pawn :
    public Piece
{
private:
    mutable bool firstMove;
public:
    Pawn(bool white, int x, int y);
    void moveTo(int xCord, int yCord);
    bool canMove(int atemptX, int atemptY, const Board& b) const;
    void getPossibleMoves(std::vector<Move>& moves, const Board& b) const;
};

