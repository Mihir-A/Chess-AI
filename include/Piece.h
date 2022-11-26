#pragma once
#include "SFML/Graphics.hpp"
#include <string>
#include <unordered_map>
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
    void setIsWhite(bool w);
    bool isWhite() const;
    const sf::Texture& getTexture() const;
    Type getPieceType() const;
    virtual void getPossibleMoves(std::vector<Move> &moves, const Board &b) const = 0;
    static bool onBoard(int p);
    static void loadTextures();
    static void unloadTextures();

protected:
    Piece(bool white, int x, int y, Type pieceType);
    bool hasMoved;
    bool white;
    int x;
    int y;
    bool dead;
    Type pieceType;
    static std::unordered_map<std::string, sf::Texture *> textures;
    inline static bool texturesLoaded = false;
};
