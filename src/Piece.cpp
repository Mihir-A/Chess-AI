#include "Piece.h"
Piece::Piece(bool white, int x, int y)
{
    this->x = x;
    this->y = y;
    this->white = white;
}

bool Piece::isWhite() const
{
    return white;
}

bool Piece::canMove(int atemptX, int atemptY, const Board& b) const
{
    /*if (o.getPiece() == nullptr)
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
    }*/
    return true;
}

 const sf::Texture& Piece::getTexture() const
{
    return texture;
}

 int Piece::getX() const {
     return x;
 }

 int Piece::getY() const {
     return y;
 }

 void Piece::moveTo(int xCord, int yCord) {
     this->x = xCord;
     this->y = yCord;
 }