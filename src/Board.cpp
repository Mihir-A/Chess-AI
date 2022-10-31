#include "Board.h"
#include "King.h"
#include "Knight.h"
#include "Queen.h"
#include "Bishop.h"
#include "Rook.h"
#include "Pawn.h"
#include "Move.h"

Board::Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{}

Board::Board(const std::string& fenStr)
{
    whitePieces.reserve(16);
    blackPieces.reserve(16);
    decipherFen(fenStr);
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
    if (b[m.getMovingY()][m.getMovingX()] != nullptr) {
        b[m.getMovingY()][m.getMovingX()]->setKill(true);
    }

    //This cast gets rid of const
    b[m.getMovingY()][m.getMovingX()] = const_cast<Piece *>(m.getMovingPiece());
    b[m.getMovingY()][m.getMovingX()]->moveTo(m.getMovingX(), m.getMovingY());

    b[m.getTargetedY()][m.getTargetedX()] = nullptr;
}

void Board::unmakeMove(const Move &m)
{
    if (m.getTargetedPiece()) {
        b[m.getMovingY()][m.getMovingX()] = const_cast<Piece *>(m.getTargetedPiece());
        b[m.getMovingY()][m.getMovingX()]->setKill(false);
        b[m.getMovingY()][m.getMovingX()]->moveTo(m.getMovingX(), m.getMovingY());
        b[m.getMovingY()][m.getMovingX()]->setHasMoved(m.isTargetedFirst());
    }
    else {
        b[m.getMovingY()][m.getMovingX()] = nullptr;
    }

    b[m.getTargetedY()][m.getTargetedX()] = const_cast<Piece *>(m.getMovingPiece());
    b[m.getTargetedY()][m.getTargetedX()]->moveTo(m.getTargetedX(), m.getTargetedY());
    b[m.getTargetedY()][m.getTargetedX()]->setHasMoved(m.isMovingFirst());
}

const std::vector<const Piece *>& Board::getWhitePieces() const
{
    return whitePieces;
}

const std::vector<const Piece *>& Board::getBlackPieces() const
{
    return blackPieces;
}

void Board::decipherFenBoard(std::string::const_iterator& it)
{
    int px = 0, py = 0;
    for (; true ; ++it) {
        const int num = *it - '0';

        if (num >= 1 && num <=8) {
            px += num - 1;
        }
        else if (*it == '/') {
            py++;
            px = 0;
        }else {
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
            if(isupper(*it)) {
                whitePieces.push_back(getPiece(px, py));
            }else {
                blackPieces.push_back(getPiece(px, py));
            }
            px++;
        }

        if (px == 8 && py == 7) {
            break;
        }
    }
}

void Board::decipherFen(const std::string &fen)
{
    for (auto& row : b) {
        std::ranges::fill(row, nullptr);
    }

    auto it = fen.begin();

    decipherFenBoard(it);
}

