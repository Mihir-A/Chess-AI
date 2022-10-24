#include "Game.h"
#include "Piece.h"
#include <algorithm>
#include <iostream>

Game::Game()
{
    board = Board();
    windowSize = 800;
	window.create(sf::VideoMode(windowSize, windowSize), "Chess", sf::Style::Titlebar + sf::Style::Close + sf::Style::Resize);
    grid.setSize(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f));
    grid.setFillColor(sf::Color(181, 136, 99));// Tan color of the board
    heldPiece = nullptr;
    
    whitePieces.reserve(16);
    blackPieces.reserve(16);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 8; j++) {
            whitePieces.push_back(board.getPiece(j, i + 6));
            blackPieces.push_back(board.getPiece(j, i));
        }
    }
    whiteTurn = true;
    getMoves();
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

            else if (event.type == sf::Event::MouseButtonPressed) {

                //X and Y cordinates of the grid where the mouse is clicked
                const int xCord = event.mouseButton.x / (windowSize / 8);
                const int yCord = event.mouseButton.y / (windowSize / 8);

                if (board.getPiece(xCord, yCord) != nullptr) {
                    //Finds piece that the mouse is over when it clicks
                    heldPiece = board.getPiece(xCord, yCord);
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece != nullptr) {

                //X and Y cordinates of the grid where the mouse is clicked
                const int xCord = event.mouseButton.x / (windowSize / 8);
                const int yCord = event.mouseButton.y / (windowSize / 8);

                if (xCord < 8 && xCord > -1 && yCord < 8 && yCord > -1){

                    Move attemptedMove = Move(heldPiece, heldPiece->getX(), heldPiece->getY(), xCord, yCord);
                    
                    if (canMove(attemptedMove) && heldPiece->isWhite() == whiteTurn) {
                        board.makeMove(attemptedMove);
                        whiteTurn = !whiteTurn;
                        getMoves();
                    }
                }
                heldPiece = nullptr;
            }

            //Makes sure the window is always a square
            if (window.getSize().x != windowSize) {
                windowSize = window.getSize().x;
                window.setSize(sf::Vector2u(windowSize, windowSize));
            }
            else if (window.getSize().y != windowSize) {
                windowSize = window.getSize().y;
                window.setSize(sf::Vector2u(windowSize, windowSize));
            }

            //std::cout << sf::Mouse::getPosition(window).x << " y: " << sf::Mouse::getPosition(window).y << " windowx: " << window.getSize().x << " y: " << window.getSize().y << '\n';
            draw();
        }
	}
}

void Game::draw()
{
    window.clear(sf::Color(240, 217, 181));
   
    float gSize = grid.getSize().x;
    grid.setPosition(gSize, 0.0f);

    //Draws the grid tiles
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            window.draw(grid);
            grid.move(gSize * 2, 0.0f);
        }

        if (i % 2 == 0) 
            grid.setPosition(0.0f, gSize * (i + 1));
        else
            grid.setPosition(gSize, gSize * (i + 1));
    }

    sf::Sprite heldSprite;

    //Draws the pieces
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (board.getPiece(j, i) != nullptr) {
                if (board.getPiece(j, i) == heldPiece && heldPiece != nullptr) {
                    //This sets position of the held piece
                    const int offset = 50;
                    heldSprite.setTexture(heldPiece->getTexture());
                    heldSprite.setPosition(sf::Mouse::getPosition(window).x * 800.0f / windowSize - offset, sf::Mouse::getPosition(window).y * 800.0f / windowSize - offset);
                }
                else {
                    sf::Sprite s;
                    s.setTexture(board.getPiece(j, i)->getTexture());
                    s.setPosition(gSize * j, gSize * i);
                    window.draw(s);
                }
                
            }
        }
    }

    //The held piece should be drawn over others
    window.draw(heldSprite);
    window.display();
}


void Game::makeMove(Move s) 
{
    
}

void Game::getMoves() 
{
    std::vector<Move>& playerMoves = whiteTurn ? whiteMoves : blackMoves;
    const std::vector<const Piece*>& playerPieces = whiteTurn ? whitePieces : blackPieces;

    playerMoves.clear();
    for (auto piece : playerPieces) {
        piece->getPossibleMoves(playerMoves, board);
    }
}

bool Game::canMove(const Move& m) 
{
    const std::vector<Move>& posibleMoves = whiteTurn ? whiteMoves : blackMoves;

    for (const Move& move : posibleMoves) {
        if (m == move) {
            return true;
        }
    }

    return false;
}
