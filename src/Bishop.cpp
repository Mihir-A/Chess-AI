#include "Bishop.h"
#include "Move.h"
#include "Board.h"
Bishop::Bishop(bool white, int x, int y)
    : Piece(white, x, y, "Bishop")
{
    if (white)
    {
        texture.loadFromFile("assets/piece/wb.png");
    }

    else
    {
        texture.loadFromFile("assets/piece/bb.png");
    }
}

void Bishop::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{

    // Down right
    for (int d = 1; d < 10; d++)
    {
        const int newX = x + d;
        const int newY = y + d;

        if (!onBoard(newX) || !onBoard(newY))
        {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr)
        {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else
        {
            if (b.getPiece(newX, newY)->isWhite() != white)
            {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // Down left
    for (int d = 1; d < 10; d++)
    {
        const int newX = x - d;
        const int newY = y + d;

        if (!onBoard(newX) || !onBoard(newY))
        {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr)
        {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else
        {
            if (b.getPiece(newX, newY)->isWhite() != white)
            {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // up right
    for (int d = 1; d < 10; d++)
    {
        const int newX = x + d;
        const int newY = y - d;

        if (!onBoard(newX) || !onBoard(newY))
        {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr)
        {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else
        {
            if (b.getPiece(newX, newY)->isWhite() != white)
            {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }

    // up left
    for (int d = 1; d < 10; d++)
    {
        const int newX = x - d;
        const int newY = y - d;

        if (!onBoard(newX) || !onBoard(newY))
        {
            break;
        }

        if (b.getPiece(newX, newY) == nullptr)
        {
            moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
        }
        else
        {
            if (b.getPiece(newX, newY)->isWhite() != white)
            {
                moves.emplace_back(this, b.getPiece(newX, newY), x, y, newX, newY);
            }
            break;
        }
    }
}