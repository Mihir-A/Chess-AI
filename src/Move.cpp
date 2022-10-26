#include "Move.h"

Move::Move(const Piece *p, int oldX, int oldY, int newX, int newY)
    : p(p)
{
	this->oldX = oldX;
	this->oldY = oldY;
	this->newX = newX;
	this->newY = newY;
}

const Piece *Move::getPiece() const
{
	return p;
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
	return (other.newX == newX && other.newY == newY && other.oldX == oldX && other.oldY == oldY && other.p == p);
}