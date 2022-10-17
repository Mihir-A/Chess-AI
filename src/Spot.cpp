#include "Spot.h"
#include "Piece.h"

Spot::Spot()
{
}

Spot::Spot(int x, int y, Piece* p)
{
    this->x = x;
    this->y = y;
    this->p = p;
}

int Spot::getX() const
{
    return x;
}

int Spot::getY() const
{
    return y;
}

Piece* Spot::getPiece() const
{
    return p;
}
