#pragma once
#include "Board.h"
#include "Move.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

class Game
{
public:
	Game();
	void play();
	void draw();
	void makeMove(Move s);
private:
	unsigned int windowSize;
	sf::RenderWindow window;
	Board board;
	sf::RectangleShape grid;
	const Piece *heldPiece;
	bool held;
};

