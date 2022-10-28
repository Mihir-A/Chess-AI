#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "Pawn.h"
#include "Move.h"

Board::Board()
{
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            this->b[i][j] = nullptr;
        }
    }
    b[0][4] = new King(false, 4, 0);
    b[7][4] = new King(true, 4, 7);

    b[0][3] = new Queen(false, 3, 0);
    b[7][3] = new Queen(true, 3, 7);

    b[0][1] = new Knight(false, 1, 0);
    b[0][6] = new Knight(false, 6, 0);
    b[7][1] = new Knight(true, 1, 7);
    b[7][6] = new Knight(true, 6, 7);

    b[0][2] = new Bishop(false, 2, 0);
    b[0][5] = new Bishop(false, 5, 0);
    b[7][2] = new Bishop(true, 2, 7);
    b[7][5] = new Bishop(true, 5, 7);

    b[0][0] = new Rook(false, 0, 0);
    b[0][7] = new Rook(false, 7, 0);
    b[7][0] = new Rook(true, 0, 7);
    b[7][7] = new Rook(true, 7, 7);

    for (int i = 0; i < 8; i++) {
        b[1][i] = new Pawn(false, i, 1);
        b[6][i] = new Pawn(true, i, 6);
    }

    whitePieces.reserve(16);
    blackPieces.reserve(16);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            whitePieces.push_back(getPiece(j, i + 6));
            blackPieces.push_back(getPiece(j, i));
        }
    }
}

Board::~Board()
{
    for (const auto piece : whitePieces) {
        delete piece;
    }
    for (const auto piece : blackPieces) {
        delete piece;
    }
}

const Piece* Board::getPiece(unsigned int x, unsigned int y) const
{
    return b[y][x];
}

void Board::makeMove(const Move &m)
{
    if (b[m.getNewY()][m.getNewX()] != nullptr) {
        b[m.getNewY()][m.getNewX()]->setKill(true);
    }

    //This cast gets rid of const
    b[m.getNewY()][m.getNewX()] = const_cast<Piece *>(m.getNewPiece());
    b[m.getNewY()][m.getNewX()]->moveTo(m.getNewX(), m.getNewY());

    b[m.getOldY()][m.getOldX()] = nullptr;
}

void Board::unmakeMove(const Move &m)
{
    if (m.getOldPiece()) {
        const_cast<Piece *>(m.getOldPiece())->setKill(false);
        b[m.getNewY()][m.getNewX()] = const_cast<Piece *>(m.getOldPiece());
        b[m.getNewY()][m.getNewX()]->moveTo(m.getNewX(), m.getNewY());
    }
    else {
        b[m.getNewY()][m.getNewX()] = nullptr;
    }

    b[m.getOldY()][m.getOldX()] = const_cast<Piece *>(m.getNewPiece());
    b[m.getOldY()][m.getOldX()]->moveTo(m.getOldX(), m.getOldY());
}

const std::vector<const Piece *>& Board::getWhitePieces() const
{
    return whitePieces;
}

const std::vector<const Piece *>& Board::getBlackPieces() const
{
    return blackPieces;
}

void Board::setPieceNull(int x, int y)
{
    b[y][x] = nullptr;
}
