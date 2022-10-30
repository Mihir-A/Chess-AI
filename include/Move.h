#pragma once
#include "Piece.h"

class Move
{
private:
    const Piece *movingPiece, *targetedPiece;
    int oldX, oldY, newX, newY;
    bool firstMoveMoving, firstMoveTargeted;
public:
    Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY);
    const Piece* getMovingPiece() const;
    const Piece* getTargetedPiece() const;
    bool isMovingFirst() const;
    bool isTargetedFirst() const;
    int getTargetedX() const;
    int getTargetedY() const;
    int getMovingX() const;
    int getMovingY() const;
    bool operator==(const Move &other) const;
};
