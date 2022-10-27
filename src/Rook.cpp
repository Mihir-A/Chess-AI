#include "Rook.h"
#include "Move.h"
#include "Board.h"
Rook::Rook(bool white, int x, int y)
    : Piece(white, x, y, "Rook")
{
    if (white)
        texture.loadFromFile("assets/piece/wr.png");

    else
        texture.loadFromFile("assets/piece/br.png");
}

void Rook::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{

    // keeps stepping right
    for (int dx = 1; dx < 8; dx++)
    {
        if (!onBoard(dx + x))
            break;
        if (b.getPiece(dx + x, y) == nullptr)
            moves.emplace_back(this, x, y, x + dx, y);
        else
        {
            if (b.getPiece(dx + x, y)->isWhite() != white)
                moves.emplace_back(this, x, y, x + dx, y);
            break;
        }
    }

    // keeps stepping left
    for (int dx = -1; dx > -8; dx--)
    {
        if (!onBoard(dx + x))
            break;
        if (b.getPiece(dx + x, y) == nullptr)
            moves.emplace_back(this, x, y, x + dx, y);
        else
        {
            if (b.getPiece(dx + x, y)->isWhite() != white)
                moves.emplace_back(this, x, y, x + dx, y);
            break;
        }
    }

    // keeps stepping up
    for (int dy = -1; dy > -8; dy--)
    {
        if (!onBoard(dy + y))
            break;
        if (b.getPiece(x, y + dy) == nullptr)
            moves.emplace_back(this, x, y, x, y + dy);
        else
        {
            if (b.getPiece(x, y + dy)->isWhite() != white)
                moves.emplace_back(this, x, y, x, y + dy);
            break;
        }
    }
    // keeps stepping down
    for (int dy = 1; dy < 8; dy++)
    {
        if (!onBoard(dy + y))
            break;
        if (b.getPiece(x, y + dy) == nullptr)
            moves.emplace_back(this, x, y, x, y + dy);
        else
        {
            if (b.getPiece(x, y + dy)->isWhite() != white)
                moves.emplace_back(this, x, y, x, y + dy);
            break;
        }
    }
}