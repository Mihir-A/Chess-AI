#pragma once
#include <vector>
#include <array>
#include "Board.h"
#include "Move.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

class Game
{
public:
	Game();
	void play();
private:
	void draw();
	void makeMove(Move s);
	void getMoves();
	unsigned int windowSize;
	sf::RenderWindow window;
	Board board;
	sf::RectangleShape grid;
	const Piece *heldPiece;
	bool whiteTurn;
	std::vector<Move> whiteMoves;
	std::vector<Move> blackMoves;
	std::vector<const Piece*> whitePieces;
	std::vector<const Piece*> blackPieces;
};

