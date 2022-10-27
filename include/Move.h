#pragma once
#include "Piece.h"
class Move
{
private:
    const Piece *p;
    int oldX, oldY, newX, newY;

public:
    Move(const Piece *p, int oldX, int oldY, int newX, int newY);
    const Piece *getPiece() const;
    int getOldX() const;
    int getOldY() const;
    int getNewX() const;
    int getNewY() const;
    bool operator==(const Move &other) const;
};
