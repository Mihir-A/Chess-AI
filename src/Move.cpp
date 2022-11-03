#include "Move.h"

Move::Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY)
    : Move(movingPiece, targetedPiece, oldX, oldY, newX, newY, MoveType::Normal)
{}

Move::Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY, MoveType moveType)
    : movingPiece(movingPiece), targetedPiece(targetedPiece), oldX(oldX), oldY(oldY), newX(newX), newY(newY), firstMoveMoving(movingPiece->getHasMoved()), moveType(moveType)
{
    if (targetedPiece) {
        firstMoveTargeted = targetedPiece->getHasMoved();
    }
}

const Piece* Move::getMovingPiece() const
{
    return movingPiece;
}

const Piece* Move::getTargetedPiece() const
{
    return targetedPiece;
}

int Move::getTargetedX() const
{
    return oldX;
}

bool Move::isMovingFirst() const
{
    return firstMoveMoving;
}

bool Move::isTargetedFirst() const
{
    return firstMoveTargeted;
}

int Move::getTargetedY() const
{
    return oldY;
}

int Move::getMovingY() const
{
    return newY;
}

int Move::getMovingX() const
{
    return newX;
}


bool Move::operator==(const Move &other) const
{
    return (other.newX == newX && other.newY == newY && other.oldX == oldX && other.oldY == oldY && other.movingPiece == movingPiece);
}

Move::MoveType Move::getMoveType() const
{
    return moveType;
}
