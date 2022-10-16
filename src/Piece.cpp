#include "Piece.h"
#include "Spot.h"

Piece::Piece(bool white)
{
    this->white = white;
}

bool Piece::isWhite()
{
    return white;
}

bool Piece::canMove(Spot o, Spot n, Board b)
{
    return false;
}
