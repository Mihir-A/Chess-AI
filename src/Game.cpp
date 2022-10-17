#include "Game.h"
#include "King.h"

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

            
            draw();
        }
	}
}

void Game::draw()
{
    window.clear();

    Piece* i = board.getSpot(0, 0).getPiece();
    sf::Sprite x(i->getTexture());
    x.setPosition(10, 10);
    window.draw(x);
    window.display();
}
