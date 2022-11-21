#pragma once
#include "AiPlayer.h"

#include <vector>
#include "Board.h"
#include "Move.h"
#include "Node.h"
#include "SFML/Graphics.hpp"

#include <future>

class Game
{
public:
    Game();
    void aiTurn();
    void play();
    void drawLoop();

private:
    void draw();
    void drawYellowSquare(float x, float y);
    void drawRedSquare(float x, float y);
    void checkGameOver();
    void getMoves();
    bool canMove(Move &m) const;
    void reset();
    void basicWindowM(std::future<void>& f);
    unsigned int windowSize;
    sf::RenderWindow window;
    Board board;
    sf::RectangleShape brownSquare;
    sf::RectangleShape yellowSquare;
    sf::RectangleShape redSquare;
    sf::RectangleShape highlightSquare;
    sf::CircleShape moveHint;
    const Piece* heldPiece;
    const Piece* recentPiece;
    bool inCheck;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<Move> playedMoves;
    bool gameOver;
    AiPlayer ai;
    bool aiStarted;
};
