#pragma once
#include "Board.h"
class Game
{
public:
	Game();
	void play();
	void draw();
private:
	sf::RenderWindow window;
	Board board;
};

