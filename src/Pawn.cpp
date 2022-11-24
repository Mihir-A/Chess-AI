#include "Pawn.h"
#include "Move.h"
#include "Board.h"
#include "Queen.h"

Pawn::Pawn(bool white, int x, int y)
    : Piece(white, x, y, Type::Pawn)
{
    if (white) {
        texture.loadFromFile("assets/piece/wp.png");

    }

    else {
        texture.loadFromFile("assets/piece/bp.png");
    }
}

void Pawn::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{
    const int yDir = white ? -1 : 1;
    const int promoY = white ? 0 : 7;

    // double pawn push
    if (onBoard(y + yDir * 2)) {
        if (b.getPiece(x, y + yDir * 2) == nullptr && !hasMoved && b.getPiece(x, y + yDir) == nullptr) {
            moves.emplace_back(this, b.getPiece(x, y + yDir * 2), x, y, x, y + yDir * 2);
        }
    }
    // single pawn push
    if (onBoard(y + yDir)) {
        if (b.getPiece(x, y + yDir) == nullptr) {
            if (y + yDir != promoY) {
                moves.emplace_back(this, b.getPiece(x, y + yDir), x, y, x, y + yDir);
            }else {
                moves.emplace_back(this, nullptr, x, y, x, promoY, Move::MoveType::Promotion);
            }
            
        }
    }
    // pawn right capture
    if (onBoard(y + yDir) && onBoard(x + 1)) {
        if (b.getPiece(x + 1, y + yDir) && b.getPiece(x + 1, y + yDir)->isWhite() != this->isWhite()) {
            if (y + yDir != promoY) {
                moves.emplace_back(this, b.getPiece(x + 1, y + yDir), x, y, x + 1, y + yDir);
            }else {
                moves.emplace_back(this, b.getPiece(x + 1, y + yDir), x, y, x + 1, y + yDir, Move::MoveType::Promotion);
            }
        }
    }
    // pawn left capture
    if (onBoard(y + yDir) && onBoard(x - 1)) {
        if (b.getPiece(x - 1, y + yDir) && b.getPiece(x - 1, y + yDir)->isWhite() != this->isWhite()) {
            if (y + yDir != promoY) {
                moves.emplace_back(this, b.getPiece(x - 1, y + yDir), x, y, x - 1, y + yDir);
            }else {
                moves.emplace_back(this, b.getPiece(x - 1, y + yDir), x, y, x - 1, y + yDir, Move::MoveType::Promotion);
            }
        }
    }
}
