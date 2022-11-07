#pragma once
#include <vector>
#include "Board.h"
#include "Move.h"
#include "SFML/Graphics.hpp"

class Game
{
public:
    Game();
    void play();

private:
    void draw();
    void drawYellowSquare(float x, float y);
    void drawRedSquare(float x, float y);
    void getMoves();
    bool canMove(Move &m) const;
    unsigned int windowSize;
    sf::RenderWindow window;
    Board board;
    sf::RectangleShape brownSquare;
    sf::RectangleShape yellowSquare;
    sf::RectangleShape redSquare;
    sf::CircleShape moveHint;
    const Piece* heldPiece;
    const Piece* recentPiece;
    bool inCheck;
    bool whiteTurn;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<Move> playedMoves;
};
