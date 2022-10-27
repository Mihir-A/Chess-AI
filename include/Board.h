#pragma once
#include <array>
#include "Piece.h"
class Move;
class Board
{
private:
    std::array<std::array<Piece *, 8>, 8> b{};

public:
    Board();
    const Piece* getPiece(unsigned int x, unsigned int y) const;
    void setPieceNull(int x, int y);
    void makeMove(const Move &m);
    void unmakeMove(const Move &m);
};
