#pragma once
#include "Piece.h"

class Move
{
private:
    const Piece *newPiece, *oldPiece;
    int oldX, oldY, newX, newY;

public:
    Move(const Piece* newPiece, const Piece* oldPiece, int oldX, int oldY, int newX, int newY);
    const Piece* getNewPiece() const;
    const Piece* getOldPiece() const;
    int getOldX() const;
    int getOldY() const;
    int getNewX() const;
    int getNewY() const;
    bool operator==(const Move &other) const;
};
