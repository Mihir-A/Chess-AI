#include "Move.h"

Move::Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY)
    : Move(movingPiece, targetedPiece, oldX, oldY, newX, newY, MoveType::Normal)
{}

Move::Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY, MoveType moveType)
    : movingPiece(movingPiece), targetedPiece(targetedPiece), oldX(oldX), oldY(oldY), newX(newX), newY(newY), firstMoveMoving(movingPiece->getHasMoved()), moveType(moveType)
{
    if (targetedPiece) {
        firstMoveTargeted = targetedPiece->getHasMoved();
    }else {
        firstMoveTargeted = false;
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

int Move::getOldX() const
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
 //   if(other.getMoveType() == MoveType::Castle || getMoveType() == MoveType::Castle) {
   //     //Since checks' second piece is the rook not the king move attempt. The check needs to be different
    //    auto& castleMove = getMoveType() != MoveType::Castle ? other : *this;
     //   auto& otherMove = getMoveType() == MoveType::Castle ? other : *this;
   // }else {
        return (other.newX == newX && other.newY == newY && other.oldX == oldX && other.oldY == oldY && other.movingPiece == movingPiece);
  //  }
}

Move::MoveType Move::getMoveType() const
{
    return moveType;
}

void Move::setMoveType(const MoveType moveType)
{
    this->moveType = moveType;
}
