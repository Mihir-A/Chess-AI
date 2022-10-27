#include "Move.h"

Move::Move(const Piece* newPiece, const Piece* oldPiece, int oldX, int oldY, int newX, int newY)
    : newPiece(newPiece), oldPiece(oldPiece), oldX(oldX), oldY(oldY), newX(newX), newY(newY)
{}

const Piece* Move::getNewPiece() const
{
    return newPiece;
}

const Piece* Move::getOldPiece() const
{
    return oldPiece;
}

int Move::getOldX() const
{
    return oldX;
}

int Move::getOldY() const
{
    return oldY;
}

int Move::getNewY() const
{
    return newY;
}

int Move::getNewX() const
{
    return newX;
}


bool Move::operator==(const Move &other) const
{
    return (other.newX == newX && other.newY == newY && other.oldX == oldX && other.oldY == oldY && other.newPiece == newPiece);
}
