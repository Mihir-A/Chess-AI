#pragma once
#include "Piece.h"

class Move
{
public:
    enum class MoveType { Normal, Castle, EnPassant, Promotion, Undefined };

    Move();
    Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY);
    Move(const Piece* movingPiece, const Piece* targetedPiece, int oldX, int oldY, int newX, int newY, MoveType moveType);
    const Piece* getMovingPiece() const;
    const Piece* getTargetedPiece() const;
    bool isMovingFirst() const;
    bool isTargetedFirst() const;
    int getOldX() const;
    int getOldY() const;
    int getNewX() const;
    int getNewY() const;
    bool operator==(const Move &other) const;
    MoveType getMoveType() const;
    void setMoveType(MoveType moveType);
    int moveValue;// For ai
    friend bool operator<(const Move& lhs, const Move& rhs)
    {
        return lhs.moveValue < rhs.moveValue;
    }

private:
    const Piece *movingPiece, *targetedPiece;
    int oldX, oldY, newX, newY;
    
    bool firstMoveMoving, firstMoveTargeted;
    MoveType moveType;
};
