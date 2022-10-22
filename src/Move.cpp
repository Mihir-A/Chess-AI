#include "Move.h"

Move::Move(const Piece* p, int oldX, int oldY, int newX, int newY) 
	:p(p)
{
	this->oldX = oldX;
	this->oldY = oldY;
	this->newX = newX;
	this->newY = newY;
}

const Piece* Move::getPiece()
{
	return p;
}

int Move::getOldX() {
	return oldX;
}

int Move::getOldY() {
	return oldY;
}

int Move::getNewY() {
	return newY;
}

int Move::getNewX() {
	return newX;
}