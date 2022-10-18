#pragma once
#include "Board.h"
#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

class Game
{
public:
	Game();
	void play();
	void draw();
private:
	unsigned int windowSize;
	sf::RenderWindow window;
	Board board;
	sf::RectangleShape grid;
	Spot heldPiece;
	bool held;
};

