#include "King.h"
#include "Move.h"
#include "Board.h"

King::King(bool white, int x, int y)
    : Piece(white, x, y, "King")
{
    if (white)
    {
        texture.loadFromFile("assets/piece/wk.png");
    }

    else
    {
        texture.loadFromFile("assets/piece/bk.png");
    }
}

void King::getPossibleMoves(std::vector<Move> &moves, const Board &b) const
{
    constexpr int offsets[3] = {-1, 0, 1};

    for (const int xOffset : offsets)
    {
        for (const int yOffset : offsets)
        {
            if (x + xOffset < 8 && x + xOffset > -1 && y + yOffset < 8 && y + yOffset > -1)
            {
                if (b.getPiece(x + xOffset, y + yOffset) == nullptr)
                {
                    moves.emplace_back(this, b.getPiece(x + xOffset, y + yOffset), x, y, x + xOffset, y + yOffset);
                }
                else if (b.getPiece(x + xOffset, y + yOffset)->isWhite() != this->isWhite())
                {
                    moves.emplace_back(this, b.getPiece(x + xOffset, y + yOffset), x, y, x + xOffset, y + yOffset);
                }
            }
        }
    }
}
