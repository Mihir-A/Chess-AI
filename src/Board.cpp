#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "Pawn.h"
#include "Move.h"
#include <algorithm>
//"k7/8/8/8/8/8/P7/R3K1NR w KQkq - 0 1"
//"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
//"r3k2r/p1ppqpb/bn2pnp/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{}

Board::~Board()
{
    for (const auto piece : whitePieces) {
        delete piece;
    }
    for (const auto piece : blackPieces) {
        delete piece;
    }
    whitePieces.clear();
    blackPieces.clear();

    for (auto &row : b) {
        row.fill(nullptr);
    }
}

Board::Board(const std::string &fenStr)
{
    whitePieces.reserve(16);
    blackPieces.reserve(16);
    decipherFen(fenStr);
    whiteTurn = true;

    int pawnCount = 0;
    for (const auto p : whitePieces) {
        if (p->getPieceType() == Piece::Type::Pawn) {
            pawnCount++;
        }
    }
    for (const auto p : blackPieces) {
        if (p->getPieceType() == Piece::Type::Pawn) {
            pawnCount++;
        }
    }

    queens.reserve(pawnCount);
    for (int i = 0; i < pawnCount; i ++) {
        queens.emplace_back(new Queen(false, -1, -1), false);
    }
}

const Piece* Board::getPiece(unsigned int x, unsigned int y) const
{
    return b[y][x];
}

void Board::makeMove(const Move &m)
{
    if (m.getMoveType() == Move::MoveType::Normal) {
        if (b[m.getNewY()][m.getNewX()] != nullptr) {
            b[m.getNewY()][m.getNewX()]->setKill(true);
        }

        //This cast gets rid of const
        movePiece(const_cast<Piece *>(m.getMovingPiece()), m.getNewX(), m.getNewY());

        b[m.getOldY()][m.getOldX()] = nullptr;
    }
    else if (m.getMoveType() == Move::MoveType::Castle) {
        // Moves King
        movePiece(const_cast<Piece *>(m.getMovingPiece()), m.getNewX(), m.getNewY());
        b[m.getOldY()][m.getOldX()] = nullptr;

        //Rook moves depending on the side
        if (m.getNewX() > m.getOldX()) {
            //Moves Rook
            movePiece(const_cast<Piece *>(m.getTargetedPiece()), m.getNewX() - 1, m.getNewY());
            b[m.getNewY()][m.getNewX() + 1] = nullptr;
        }
        else {
            //Moves Rook
            movePiece(const_cast<Piece *>(m.getTargetedPiece()), m.getNewX() + 1, m.getNewY());
            b[m.getNewY()][m.getNewX() - 2] = nullptr;
        }
    }
    else if (m.getMoveType() == Move::MoveType::Promotion) {
        if (m.getTargetedPiece()) {
            const_cast<Piece *>(m.getTargetedPiece())->setKill(true);
        }
        b[m.getOldY()][m.getOldX()] = nullptr;
        const_cast<Piece *>(m.getMovingPiece())->setKill(true);
        b[m.getNewY()][m.getNewX()] = getPromoQueen(m.getMovingPiece()->isWhite(), m.getNewX(), m.getNewY());
        auto &pieces = m.getMovingPiece()->isWhite() ? whitePieces : blackPieces;
        pieces.push_back(b[m.getNewY()][m.getNewX()]);
    }
}

void Board::unmakeMove(const Move &m)
{
    if (m.getMoveType() == Move::MoveType::Normal) {
        if (m.getTargetedPiece()) {
            movePiece(const_cast<Piece *>(m.getTargetedPiece()), m.getNewX(), m.getNewY());
            b[m.getNewY()][m.getNewX()]->setKill(false);
            b[m.getNewY()][m.getNewX()]->setHasMoved(m.isTargetedFirst());
        }
        else {
            b[m.getNewY()][m.getNewX()] = nullptr;
        }
        movePiece(const_cast<Piece *>(m.getMovingPiece()), m.getOldX(), m.getOldY());
        b[m.getOldY()][m.getOldX()]->setHasMoved(m.isMovingFirst());
    }
    else if (m.getMoveType() == Move::MoveType::Castle) {
        // Moves King
        movePiece(const_cast<Piece *>(m.getMovingPiece()), m.getOldX(), m.getOldY());
        b[m.getNewY()][m.getNewX()] = nullptr;

        if (m.getNewX() > m.getOldX()) {
            //Moves Rook
            movePiece(const_cast<Piece *>(m.getTargetedPiece()), m.getNewX() + 1, m.getNewY());
            b[m.getNewY()][m.getNewX() - 1] = nullptr;
            b[m.getNewY()][m.getNewX() + 1]->setHasMoved(m.isTargetedFirst());
        }
        else {
            //Moves Rook
            movePiece(const_cast<Piece *>(m.getTargetedPiece()), m.getNewX() - 2, m.getNewY());
            b[m.getNewY()][m.getNewX() + 1] = nullptr;
            b[m.getNewY()][m.getNewX() - 2]->setHasMoved(m.isTargetedFirst());
        }
        b[m.getOldY()][m.getOldX()]->setHasMoved(m.isMovingFirst());
    }
    else if (m.getMoveType() == Move::MoveType::Promotion) {

        if (m.getTargetedPiece()) {
            const_cast<Piece *>(m.getTargetedPiece())->setKill(false);
        }
        const_cast<Piece *>(m.getMovingPiece())->setKill(false);
        removePromoQueen(b[m.getNewY()][m.getNewX()]);
        auto &pieces = m.getMovingPiece()->isWhite() ? whitePieces : blackPieces;
        pieces.erase(std::remove(pieces.begin(), pieces.end(), b[m.getNewY()][m.getNewX()]), pieces.end());
        b[m.getNewY()][m.getNewX()] = const_cast<Piece *>(m.getTargetedPiece());
        b[m.getOldY()][m.getOldX()] = const_cast<Piece *>(m.getMovingPiece());
    }
}

const std::vector<const Piece *>& Board::getWhitePieces() const
{
    return whitePieces;
}

const std::vector<const Piece *>& Board::getBlackPieces() const
{
    return blackPieces;
}

void Board::decipherFenBoard(std::string::const_iterator &it)
{
    int px = 0, py = 0;
    for (; true; ++it) {
        const int num = *it - '0';

        if (num >= 1 && num <= 8) {
            px += num;
        }
        else if (*it == '/') {
            py++;
            px = 0;
        }
        else {
            switch (*it) {
            default:
                break;
            case 'p':
                b[py][px] = new Pawn(false, px, py);
                break;
            case 'P':
                b[py][px] = new Pawn(true, px, py);
                break;
            case 'r':
                b[py][px] = new Rook(false, px, py);
                break;
            case 'R':
                b[py][px] = new Rook(true, px, py);
                break;
            case 'q':
                b[py][px] = new Queen(false, px, py);
                break;
            case 'Q':
                b[py][px] = new Queen(true, px, py);
                break;
            case 'k':
                b[py][px] = new King(false, px, py);
                break;
            case 'K':
                b[py][px] = new King(true, px, py);
                break;
            case 'n':
                b[py][px] = new Knight(false, px, py);
                break;
            case 'N':
                b[py][px] = new Knight(true, px, py);
                break;
            case 'b':
                b[py][px] = new Bishop(false, px, py);
                break;
            case 'B':
                b[py][px] = new Bishop(true, px, py);
                break;
            }
            if (isupper(*it)) {
                whitePieces.push_back(getPiece(px, py));
            }
            else {
                blackPieces.push_back(getPiece(px, py));
            }
            px++;
        }

        if (px == 8 && py == 7) {
            break;
        }
    }
}

void Board::movePiece(Piece* p, int x, int y)
{
    b[y][x] = p;
    b[y][x]->moveTo(x, y);
}

void Board::decipherFen(const std::string &fen)
{
    for (const auto piece : whitePieces) {
        delete piece;
    }
    for (const auto piece : blackPieces) {
        delete piece;
    }
    whitePieces.clear();
    blackPieces.clear();

    for (auto &row : b) {
        row.fill(nullptr);
    }

    auto it = fen.begin();
    decipherFenBoard(it);
}

void Board::changeTurn()
{
    whiteTurn = !whiteTurn;
}

bool Board::isWhiteTurn() const
{
    return whiteTurn;
}

void Board::setWhiteTurn(bool t)
{
    whiteTurn = t;
}

Piece* Board::getPromoQueen(bool white, int x, int y)
{
    for (auto &[piece, onBoard] : queens) {
        if (!onBoard) {
            onBoard = true;
            piece->moveTo(x, y);
            piece->setIsWhite(white);
            return piece;
        }
    }
    //Should never reach here
    return nullptr;
}

void Board::removePromoQueen(Piece* q)
{
    for (auto &[piece, onBoard] : queens) {
        if (piece == q) {
            onBoard = false;
        }
    }
}
