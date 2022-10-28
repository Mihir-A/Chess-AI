#pragma once
#include "SFML/Graphics.hpp"
#include <iostream>
#include <string>
#include <vector>
class Board;
class Move;

class Piece
{
protected:
    Piece(bool white, int x, int y, const std::string &name);
    sf::Texture texture;
    bool firstMove;
    bool white;
    int x;
    int y;
    bool dead;
    static bool onBoard(int p);
    const std::string tName;

public:
    virtual ~Piece() = default;
    int getX() const;
    int getY() const;
    virtual void moveTo(int xCord, int yCord);
    bool isDead() const;
    void setKill(bool killed);
    bool isWhite() const;
    const sf::Texture& getTexture() const;
    const std::string& getPieceType() const;
    virtual void getPossibleMoves(std::vector<Move> &moves, const Board &b) const = 0;
};
