#include "Piece.h"

//this constructor should only be called by subclasses to initilize name
Piece::Piece(bool white, int x, int y, const std::string& name)
    :tName(name)
{
    this->x = x;
    this->y = y;
    this->white = white;
    dead = false;
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

 void Piece::kill() 
 {
     dead = true;
 }

 bool Piece::onBoard(int p) const
 {
     return (p > -1 && p < 8);
}

 const std::string& Piece::getPieceType() const {
     return tName;
 }