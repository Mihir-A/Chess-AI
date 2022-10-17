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
    return false;
}

 const sf::Texture& Piece::getTexture() const
{
    return texture;
}
