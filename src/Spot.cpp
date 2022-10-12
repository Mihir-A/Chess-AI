#include "Spot.h"

Spot::Spot()
{
}

Spot::Spot(int x, int y, Piece* p)
{
    this->x = x;
    this->y = y;
    this->p = p;
}

int Spot::getX()
{
    return x;
}

int Spot::getY()
{
    return y;
}
