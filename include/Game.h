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
	sf::RenderWindow window;
	Board board;
};

