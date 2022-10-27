#include "Queen.h"
#include "Move.h"
#include "Board.h"

Queen::Queen(bool white, int x, int y)
    : Piece(white, x, y, "Queen")
{
    if (white) {
        texture.loadFromFile("assets/piece/wq.png");
    }

    else {
        texture.loadFromFile("assets/piece/bq.png");
    }
}

void Queen::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{
    // Bishop Moves:
    // Down right
    for (int d = 1; d < 10; d++) {
        const int newX = x + d;
        const int newY = y + d;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr) {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // Down left
    for (int d = 1; d < 10; d++) {
        const int newX = x - d;
        const int newY = y + d;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr) {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // up right
    for (int d = 1; d < 10; d++) {
        const int newX = x + d;
        const int newY = y - d;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr) {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // up left
    for (int d = 1; d < 10; d++) {
        const int newX = x - d;
        const int newY = y - d;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr) {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // Rook Moves:
    // keeps stepping right
    for (int dx = 1; dx < 8; dx++) {
        if (!onBoard(dx + x)) {
            break;
        }
        if (b.getPiece(dx + x, y) == nullptr) {
            moves.emplace_back(this, b.getPiece(x + dx, y), x, y, x + dx, y);
        }
        else {
            if (b.getPiece(dx + x, y)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(x + dx, y), x, y, x + dx, y);
            }
            break;
        }
    }

    // keeps stepping left
    for (int dx = -1; dx > -8; dx--) {
        if (!onBoard(dx + x)) {
            break;
        }
        if (b.getPiece(dx + x, y) == nullptr) {
            moves.emplace_back(this, b.getPiece(x + dx, y), x, y, x + dx, y);
        }
        else {
            if (b.getPiece(dx + x, y)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(x + dx, y), x, y, x + dx, y);
            }
            break;
        }
    }

    // keeps stepping up
    for (int dy = -1; dy > -8; dy--) {
        if (!onBoard(dy + y)) {
            break;
        }
        if (b.getPiece(x, y + dy) == nullptr) {
            moves.emplace_back(this, b.getPiece(x, y + dy), x, y, x, y + dy);
        }
        else {
            if (b.getPiece(x, y + dy)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(x, y + dy), x, y, x, y + dy);
            }
            break;
        }
    }
    // keeps stepping down
    for (int dy = 1; dy < 8; dy++) {
        if (!onBoard(dy + y)) {
            break;
        }
        if (b.getPiece(x, y + dy) == nullptr) {
            moves.emplace_back(this, b.getPiece(x, y + dy), x, y, x, y + dy);
        }
        else {
            if (b.getPiece(x, y + dy)->isWhite() != white) {
                moves.emplace_back(this, b.getPiece(x, y + dy), x, y, x, y + dy);
            }
            break;
        }
    }
}
