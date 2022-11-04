#include "King.h"
#include "Move.h"
#include "Board.h"

King::King(bool white, int x, int y)
    : Piece(white, x, y, "King")
{
    if (white) {
        texture.loadFromFile("assets/piece/wk.png");
    }

    else {
        texture.loadFromFile("assets/piece/bk.png");
    }
}

void King::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{
    constexpr int offsets[3] = {-1, 0, 1};

    for (const int xOffset : offsets) {
        for (const int yOffset : offsets) {
            if (x + xOffset < 8 && x + xOffset > -1 && y + yOffset < 8 && y + yOffset > -1) {
                if (b.getPiece(x + xOffset, y + yOffset) == nullptr) {
                    moves.emplace_back(this, b.getPiece(x + xOffset, y + yOffset), x, y, x + xOffset, y + yOffset);
                }
                else if (b.getPiece(x + xOffset, y + yOffset)->isWhite() != this->isWhite()) {
                    moves.emplace_back(this, b.getPiece(x + xOffset, y + yOffset), x, y, x + xOffset, y + yOffset);
                }
            }
        }
    }

    const Piece* rook = b.getPiece(y, x + 3);
    //castle 
    if(!hasMoved && rook && rook->getPieceType() == "Rook" && !rook->getHasMoved()) {
        if(!b.getPiece(x + 1, y) && !b.getPiece(x + 2, y)) {
            bool castleCheck = false;
            for (auto dx : {1, 2}) {
                Move m = Move(this, b.getPiece(x + dx, y), x, y, x + dx, y);
                const_cast<Board&>(b).makeMove(m);
                if (inCheck(b)) {
                    castleCheck = true;
                }
                const_cast<Board&>(b).unmakeMove(m);
            }
            if(!castleCheck) {
                moves.emplace_back(this, b.getPiece(x + 3, y), x, y, x + 2, y, Move::MoveType::Castle);
            }
        }
    }

}

bool King::inCheck(const Board &b) const
{
    //Will keep stepping in all directions until it finds a piece
    //Bishop/Queen
    for (int d = 1; d < 10; d++) {
        const int newX = x + d;
        const int newY = y + d;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Bishop" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
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

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Bishop" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
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

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Bishop" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
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

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Bishop" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
            }
            break;
        }
    }

    // Rook Moves:
    // keeps stepping right
    for (int dx = 1; dx < 8; dx++) {
        const int newX = x + dx;
        const int newY = y;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Rook" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
            }
            break;
        }
    }

    // keeps stepping left
    for (int dx = -1; dx > -8; dx--) {
        const int newX = x + dx;
        const int newY = y;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Rook" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
            }
            break;
        }
    }

    // keeps stepping up
    for (int dy = -1; dy > -8; dy--) {
        const int newX = x;
        const int newY = y + dy;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Rook" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
            }
            break;
        }
    }
    // keeps stepping down
    for (int dy = 1; dy < 8; dy++) {
        const int newX = x;
        const int newY = y + dy;

        if (!onBoard(newX) || !onBoard(newY)) {
            break;
        }

        if (b.getPiece(newX, newY) != nullptr) {
            if (b.getPiece(newX, newY)->isWhite() != white) {
                if (b.getPiece(newX, newY)->getPieceType() == "Rook" || b.getPiece(newX, newY)->getPieceType() == "Queen") {
                    return true;
                }
            }
            break;
        }
    }

    // Tall knight moves
    for (const int dx : {-2, 2}) {
        for (const int dy : {-1, 1}) {
            if (onBoard(x + dx) && onBoard(y + dy) && b.getPiece(x + dx, y + dy) && b.getPiece(x + dx, y + dy)->isWhite() != white && b.getPiece(x + dx, y + dy)->getPieceType() == "Knight") {
                return true;
            }
        }
    }

    // Wide knight moves
    for (const int dx : {-1, 1}) {
        for (const int dy : {-2, 2}) {
            if (onBoard(x + dx) && onBoard(y + dy) && b.getPiece(x + dx, y + dy) && b.getPiece(x + dx, y + dy)->isWhite() != white && b.getPiece(x + dx, y + dy)->getPieceType() == "Knight") {
                return true;
            }
        }
    }

    return false;
}
