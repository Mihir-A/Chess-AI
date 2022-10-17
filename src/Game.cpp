#include "Game.h"
#include "King.h"
#include<iostream>

Game::Game()
{
    board = Board();
    windowSize = 800;
	window.create(sf::VideoMode(windowSize, windowSize), "Chess", sf::Style::Titlebar + sf::Style::Close + sf::Style::Resize);
    grid.setSize(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f));
    grid.setFillColor(sf::Color(181, 136, 99));
    
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

            if (window.getSize().x != windowSize) {
                windowSize = window.getSize().x;
                window.setSize(sf::Vector2u(windowSize, windowSize));
            }
            else if (window.getSize().y != windowSize) {
                windowSize = window.getSize().y;
                window.setSize(sf::Vector2u(windowSize, windowSize));
            }

            draw();
        }
	}
}

void Game::draw()
{
    window.clear(sf::Color(240, 217, 181));
   
    int gSize = grid.getSize().x;
    grid.setPosition(gSize, 0.0f);

    //Draws the tile grid
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            window.draw(grid);
            grid.move(gSize * 2, 0.0f);
        }

        if (i % 2 == 0) 
            grid.setPosition(0.0f, gSize * (i + 1));
        else
            grid.setPosition(gSize, gSize * (i + 1));
        
        //std::cout << grid.getSize().x << " y: " << grid.getSize().y << " windowx: " << window.getSize().x << " y: " << window.getSize().y << '\n';
    }

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board.getSpot(i, j).getPiece() != nullptr) {
                sf::Sprite s;
                s.setTexture(board.getSpot(i, j).getPiece()->getTexture());
                s.setPosition(gSize * j, gSize * i);
                window.draw(s);
            }
        }
    }

    window.display();
}
