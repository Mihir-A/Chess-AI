#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include <vector>
class Board;
class Move;

class Piece
{
public:
    enum class Type { Pawn, Rook, Queen, King, Bishop, Knight };

    virtual ~Piece() = default;
    int getX() const;
    int getY() const;
    bool getHasMoved() const;
    void setHasMoved(bool moved);
    void moveTo(int xCord, int yCord);
    bool isDead() const;
    void setKill(bool killed);
    bool isWhite() const;
    const sf::Texture& getTexture() const;
    Type getPieceType() const;
    virtual void getPossibleMoves(std::vector<Move> &moves, const Board &b) const = 0;
    static bool onBoard(int p);

protected:
    Piece(bool white, int x, int y, Type pieceType);
    sf::Texture texture;
    bool hasMoved;
    bool white;
    int x;
    int y;
    bool dead;

    Type pieceType;
};
