#include "Piece.h"
#include <array>

std::unordered_map<std::string, sf::Texture *> Piece::textures;

Piece::Piece(bool white, int x, int y, Type pieceType)
    : hasMoved(false)
      , white(white)
      , x(x)
      , y(y)
      , dead(false)
      , pieceType(pieceType)
{
    loadTextures();
}

bool Piece::getHasMoved() const
{
    return hasMoved;
}

void Piece::setHasMoved(bool moved)
{
    hasMoved = moved;
}

bool Piece::isWhite() const
{
    return white;
}

const sf::Texture& Piece::getTexture() const
{
    std::string t = white ? "w" : "b";
    switch (pieceType) {
    case Type::Pawn:
        t += "p";
        break;
    case Type::Rook:
        t += "r";
        break;
    case Type::Queen:
        t += "q";
        break;
    case Type::King:
        t += "k";
        break;
    case Type::Bishop:
        t += "b";
        break;
    case Type::Knight:
        t += "n";
        break;
    }
    return *(textures.at(t));
}

Piece::Type Piece::getPieceType() const
{
    return pieceType;
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
    hasMoved = true;
}

bool Piece::isDead() const
{
    return dead;
}

void Piece::setKill(bool killed)
{
    dead = killed;
}

void Piece::setIsWhite(bool w)
{
    this->white = w;
}

bool Piece::onBoard(int p)
{
    return (p > -1 && p < 8);
}

void Piece::loadTextures()
{
    if (!texturesLoaded) {
        const std::array<std::string, 12> texturesName{"wk", "bk", "wb", "bb", "wn", "bn", "wp", "bp", "wr", "br", "wq", "bq"};

        for (const auto &s : texturesName) {
            textures[s] = new sf::Texture();
            textures[s]->loadFromFile("assets/piece/" + s + ".png");
            textures[s]->setSmooth(true);
        }
        texturesLoaded = true;
    }
}

void Piece::unloadTextures()
{
    for (auto [name, texture] : textures) {
        delete texture;
    }
    textures.clear();
}
