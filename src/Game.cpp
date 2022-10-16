#include "Game.h"

Game::Game()
{
	window.create(sf::VideoMode(800, 800), "Chess");
	board = Board();
}

void Game::play()
{
	while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
	}
}

void Game::draw()
{

}
