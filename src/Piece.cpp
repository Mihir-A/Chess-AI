#include "Piece.h"


Piece::Piece(bool white, int x, int y, const std::string &name)
    : firstMove(true)
      , white(white)
      , x(x)
      , y(y)
      , dead(false)
      , tName(name)
{
    this->texture.setSmooth(true);
}

bool Piece::isWhite() const
{
    return white;
}

const sf::Texture& Piece::getTexture() const
{
    return texture;
}

int Piece::getX() const
{
    return x;
}

int Piece::getY() const
{
    return y;
}

void Piece::moveTo(int xCord, int yCord)
{
    this->x = xCord;
    this->y = yCord;
}

bool Piece::isDead() const
{
    return dead;
}

void Piece::setKill(bool killed)
{
    dead = killed;
}

bool Piece::onBoard(int p)
{
    return (p > -1 && p < 8);
}

const std::string& Piece::getPieceType() const
{
    return tName;
}
