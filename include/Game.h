#pragma once
#include "Board.h"
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
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

