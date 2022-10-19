#include "Piece.h"
#include "Spot.h"

Piece::Piece(bool white)
{
    this->white = white;
}

bool Piece::isWhite() const
{
    return white;
}

bool Piece::canMove(const Spot& o, const Spot& n, const Board& b) const
{
    if (o.getPiece() == nullptr)
        return false;
    if (n.getPiece() != nullptr) {
        if (o.getPiece()->isWhite() == n.getPiece()->isWhite()) {
            //Checks if color is the same
            return false;
        }
    }
    else if (o.getX() == n.getX() && o.getY() == n.getY()) {
        //checks if pieces are on the same location
        return false;
    }
    return true;
}

 const sf::Texture& Piece::getTexture() const
{
    return texture;
}
