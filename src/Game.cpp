#include "Game.h"
#include "Piece.h"
#include <iostream>

Game::Game()
{
    board = Board();
    windowSize = 800;
    window.create(sf::VideoMode(windowSize, windowSize), "Chess", sf::Style::Titlebar + sf::Style::Close + sf::Style::Resize);
    brownSquare.setSize(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f));
    brownSquare.setFillColor(sf::Color(181, 136, 99));// Tan color of the board
    yellowSquare.setSize(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f));
    yellowSquare.setFillColor(sf::Color(247, 236, 91));// yellow color of the selected piece
    heldPiece = nullptr;
    recentPiece = nullptr;

    playedMoves.reserve(10);

    whiteTurn = true;
    getMoves();
}

void Game::play()
{
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            else if (event.type == sf::Event::MouseButtonPressed) {
                // X and Y coordinates of the grid where the mouse is clicked
                const unsigned int xCord = event.mouseButton.x / (windowSize / 8);
                const unsigned int yCord = event.mouseButton.y / (windowSize / 8);

                if (board.getPiece(xCord, yCord) != nullptr && board.getPiece(xCord, yCord)->isWhite() == whiteTurn) {
                    //Finds piece that the mouse is over when it clicks
                    heldPiece = board.getPiece(xCord, yCord);
                }
                else if (recentPiece != nullptr) {
                    //This runs if a piece is already selected and wants to move to the space
                    auto attemptedMove = Move(recentPiece, board.getPiece(xCord, yCord), recentPiece->getX(), recentPiece->getY(), xCord, yCord);

                    if (canMove(attemptedMove) && recentPiece->isWhite() == whiteTurn) {
                        board.makeMove(attemptedMove);
                        whiteTurn = !whiteTurn;
                        getMoves();
                        recentPiece = nullptr;
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece != nullptr) {
                // X and Y coordinate of the grid where the mouse is clicked
                const int xCord = event.mouseButton.x / (windowSize / 8);
                const int yCord = event.mouseButton.y / (windowSize / 8);

                if (xCord < 8 && xCord > -1 && yCord < 8 && yCord > -1) {
                    if (heldPiece == recentPiece) {
                        //If a piece has been clicked twice deselect it
                        recentPiece = nullptr;
                    }
                    else {
                        recentPiece = heldPiece;
                    }

                    const auto attemptedMove = Move(heldPiece, board.getPiece(xCord, yCord), heldPiece->getX(), heldPiece->getY(), xCord, yCord);

                    if (canMove(attemptedMove)) {
                        board.makeMove(attemptedMove);
                        playedMoves.push_back(attemptedMove);
                        whiteTurn = !whiteTurn;
                        getMoves();
                        recentPiece = nullptr;
                    }
                }
                heldPiece = nullptr;
            }

            // Makes sure the window is always a square
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

    const float gSize = brownSquare.getSize().x;
    brownSquare.setPosition(gSize, 0.0f);

    //Draws the grid tiles
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            window.draw(brownSquare);
            brownSquare.move(gSize * 2, 0.0f);
        }

        if (i % 2 == 0) {
            brownSquare.setPosition(0.0f, gSize * (i + 1));
        }
        else {
            brownSquare.setPosition(gSize, gSize * (i + 1));
        }
    }

    sf::Sprite heldSprite;

    // Draws the pieces
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (board.getPiece(j, i) != nullptr) {
                if (board.getPiece(j, i) == heldPiece && heldPiece != nullptr) {
                    // This sets position of the held piece
                    constexpr int offset = 50;
                    heldSprite.setTexture(heldPiece->getTexture());
                    heldSprite.setPosition(sf::Mouse::getPosition(window).x * 800.0f / windowSize - offset, sf::Mouse::getPosition(window).y * 800.0f / windowSize - offset);
                    yellowSquare.setPosition(gSize * j, gSize * i);
                    window.draw(yellowSquare);
                }
                else if (heldPiece == nullptr && board.getPiece(j, i) == recentPiece && recentPiece != nullptr) {
                    yellowSquare.setPosition(gSize * j, gSize * i);
                    window.draw(yellowSquare);
                    sf::Sprite s;
                    s.setTexture(board.getPiece(j, i)->getTexture());
                    s.setPosition(gSize * j, gSize * i);
                    window.draw(s);
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

    // The held piece should be drawn over others
    window.draw(heldSprite);
    window.display();
}

void Game::getMoves()
{
    std::vector<Move> &playerMoves = whiteTurn ? whiteMoves : blackMoves;
    const std::vector<const Piece *> &playerPieces = whiteTurn ? board.getWhitePieces() : board.getBlackPieces();

    playerMoves.clear();
    for (const auto piece : playerPieces) {
        if (!piece->isDead()) {
            piece->getPossibleMoves(playerMoves, board);
        }
    }

    std::cout << "Possible moves for " << (whiteTurn ? "White" : "Black") << ":\n";

    for (const Move &a : playerMoves) {
        std::cout << a.getNewPiece()->getPieceType() << " from " << a.getOldX() << " " << 7 - a.getOldY() << " to " << a.getNewX() << " " << 7 - a.getNewY() << '\n';
    }
    std::cout << "\n\n\n";
}

bool Game::canMove(const Move &m) const
{
    const std::vector<Move> &possibleMoves = whiteTurn ? whiteMoves : blackMoves;

    for (const Move &move : possibleMoves) {
        if (m == move) {
            return true;
        }
    }

    return false;
}
