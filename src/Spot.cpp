#include "Spot.h"
#include <iostream>
#include "Piece.h"

Spot::Spot()
{
    x = 0;
    y = 0;
    p = nullptr;
}

void Spot::deletePiece()
{
    if (p != nullptr) {
        std::cout << "Piece at " << x << " " << y << " deleted]\n";
    }
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

void Spot::setPiece(Piece* newP)
{
    this->p = newP;
}
