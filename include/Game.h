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
    unsigned int windowSize;
    sf::RenderWindow window;
    Board board;
    sf::RectangleShape brownSquare;
    sf::RectangleShape yellowSquare;
    const Piece *heldPiece;
    const Piece *recentPiece;
    bool whiteTurn;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;
    std::vector<const Piece *> whitePieces;
    std::vector<const Piece *> blackPieces;
    std::vector<Move> playedMoves;
};
