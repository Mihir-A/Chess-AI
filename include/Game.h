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
    void getMoves();
    bool canMove(const Move &m) const;
    bool findCheck(const Move &move);
    unsigned int windowSize;
    sf::RenderWindow window;
    Board board;
    sf::RectangleShape brownSquare;
    sf::RectangleShape yellowSquare;
    const Piece* heldPiece;
    const Piece* recentPiece;
    bool whiteTurn;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<Move> playedMoves;
};
