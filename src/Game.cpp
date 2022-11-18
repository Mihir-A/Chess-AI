#include "Game.h"
#include "Piece.h"
#include "King.h"
#include "Node.h"
#include <chrono>
#include <thread>
#include <future>
#include <iostream>

std::future<void> f;

Game::Game()
    : windowSize(800)
      , window(sf::VideoMode(windowSize, windowSize), "Chess", sf::Style::Titlebar + sf::Style::Close + sf::Style::Resize)
      , brownSquare(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f))
      , yellowSquare(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f))
      , redSquare(sf::Vector2f(windowSize / 8.0f, windowSize / 8.0f))
      , highlightSquare(sf::Vector2f(windowSize / 8.0f - 10, windowSize / 8.0f - 10))
      , moveHint(15)
      , heldPiece(nullptr)
      , recentPiece(nullptr)
      , gameOver(true)
      , ai(board)
{
    brownSquare.setFillColor(sf::Color(181, 136, 99));// Tan color of the board
    yellowSquare.setFillColor(sf::Color(255, 255, 0, 130));// yellow color of the selected piece
    redSquare.setFillColor(sf::Color(235, 97, 80, 204));//Color of king when in check
    highlightSquare.setFillColor(sf::Color(0, 0, 0, 0));
    highlightSquare.setOutlineColor(sf::Color(255, 255, 255, 166));
    highlightSquare.setOutlineThickness(5);
    moveHint.setFillColor(sf::Color(0, 0, 0, 25));
    moveHint.setOrigin(moveHint.getRadius(), moveHint.getRadius());
    playedMoves.reserve(10);
    getMoves();
    window.setVerticalSyncEnabled(true);
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

                if (board.getPiece(xCord, yCord) != nullptr && board.getPiece(xCord, yCord)->isWhite() == board.isWhiteTurn()) {
                    //Finds piece that the mouse is over when it clicks
                    heldPiece = board.getPiece(xCord, yCord);
                }
                else if (recentPiece != nullptr) {
                    //This runs if a piece is already selected and wants to move to the space
                    auto attemptedMove = Move(recentPiece, board.getPiece(xCord, yCord), recentPiece->getX(), recentPiece->getY(), xCord, yCord);

                    if (canMove(attemptedMove) && recentPiece->isWhite() == board.isWhiteTurn()) {
                        board.makeMove(attemptedMove);
                        playedMoves.push_back(attemptedMove);
                        board.changeTurn();
                        getMoves();
                        recentPiece = nullptr;
                    }
                }
            }
            else if (event.type == sf::Event::MouseButtonReleased && heldPiece) {
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

                    auto attemptedMove = Move(heldPiece, board.getPiece(xCord, yCord), heldPiece->getX(), heldPiece->getY(), xCord, yCord);

                    if (canMove(attemptedMove)) {
                        board.makeMove(attemptedMove);
                        playedMoves.push_back(attemptedMove);
                        board.changeTurn();
                        getMoves();
                        recentPiece = nullptr;
                    }
                }
                heldPiece = nullptr;
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                if (!playedMoves.empty()) {
                    board.unmakeMove(playedMoves[playedMoves.size() - 1]);
                    playedMoves.pop_back();
                    board.changeTurn();
                    getMoves();
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R && gameOver == true) {
                reset();
            }
            else if (event.type == sf::Event::Resized) {
                // Makes sure the window is always a square
                if (window.getSize().x != windowSize) {
                    windowSize = window.getSize().x;
                    window.setSize(sf::Vector2u(windowSize, windowSize));
                }
                else if (window.getSize().y != windowSize) {
                    windowSize = window.getSize().y;
                    window.setSize(sf::Vector2u(windowSize, windowSize));
                }
            }
        }
        draw();
        if (board.isWhiteTurn() == true ) {
            board.makeMove(ai.getBestMove());
            board.changeTurn();
            getMoves();
        }
    }
}

void Game::draw()
{
    window.clear(sf::Color(240, 217, 181));

    constexpr float gSize = 100.0f;
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

    //highlights old move
    if (!playedMoves.empty()) {
        drawYellowSquare(gSize * playedMoves[playedMoves.size() - 1].getNewX(), gSize * playedMoves[playedMoves.size() - 1].getNewY());
        drawYellowSquare(gSize * playedMoves[playedMoves.size() - 1].getOldX(), gSize * playedMoves[playedMoves.size() - 1].getOldY());
    }

    //Highlights king in red if in check
    if (inCheck) {
        for (const auto piece : board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces()) {
            if (piece->getPieceType() == Piece::Type::King) {
                drawRedSquare(piece->getX() * gSize, piece->getY() * gSize);
            }
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
                    drawYellowSquare(gSize * j, gSize * i);
                }
                else if (heldPiece == nullptr && board.getPiece(j, i) == recentPiece && recentPiece != nullptr) {
                    drawYellowSquare(gSize * j, gSize * i);
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

    //draws the piece hints
    const std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    if (heldPiece) {
        for (const auto &m : playerMoves) {
            if (m.getMovingPiece() == heldPiece) {
                moveHint.setPosition((m.getNewX() + 0.5f) * gSize, (m.getNewY() + 0.5f) * gSize);
                window.draw(moveHint);
            }
        }
    }
    else if (recentPiece) {
        for (const auto &m : playerMoves) {
            if (m.getMovingPiece() == recentPiece) {
                moveHint.setPosition((m.getNewX() + 0.5f) * gSize, (m.getNewY() + 0.5f) * gSize);
                window.draw(moveHint);
            }
        }
    }

    //Draws a box around highlighted piece
    if (heldPiece) {
        constexpr int offset = 5;
        const int mouseX = sf::Mouse::getPosition(window).x * 800 / windowSize;
        const int mouseY = sf::Mouse::getPosition(window).y * 800 / windowSize;
        //Conversion rounds Mouse to nearest box
        const int xCord = mouseX / static_cast<int>(gSize) * gSize + offset;// NOLINT
        const int yCord = mouseY / static_cast<int>(gSize) * gSize + offset;// NOLINT

        highlightSquare.setPosition(xCord, yCord);
        window.draw(highlightSquare);
    }

    // The held piece should be drawn over others
    window.draw(heldSprite);
    window.display();
}

void Game::drawYellowSquare(float x, float y)
{
    yellowSquare.setPosition(x, y);
    window.draw(yellowSquare);
}

void Game::drawRedSquare(float x, float y)
{
    redSquare.setPosition(x, y);
    window.draw(redSquare);
}

void Game::checkGameOver()
{
    const std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    if (playerMoves.empty()) {
        if (inCheck) {
            std::cout << (board.isWhiteTurn() ? "White" : "Black") << " is in checkmate";
        }
        else {
            std::cout << (board.isWhiteTurn() ? "White" : "Black") << " is in stalemate";
        }
        gameOver = true;
    }
}

void Game::getMoves()
{
    inCheck = false;
    std::vector<Move> &playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    const std::vector<const Piece *> &playerPieces = board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces();

    //gets pseudo legal moves
    playerMoves.clear();
    for (const auto piece : playerPieces) {
        if (!piece->isDead()) {
            piece->getPossibleMoves(playerMoves, board);
        }
    }

    const King* king = nullptr;

    for (const auto piece : playerPieces) {
        if (piece->getPieceType() == Piece::Type::King) {
            king = dynamic_cast<const King *>(piece);
        }
    }

    //This lambda plays all possible moves and then checks if they cause the king to be in check
    //These moves are removed from the list
    const auto it = std::remove_if(playerMoves.begin(), playerMoves.end(), [this, &king](const Move &m)
    {
        board.makeMove(m);
        if (king->inCheck(board)) {
            board.unmakeMove(m);
            return true;
        }
        board.unmakeMove(m);
        return false;
    });

    playerMoves.erase(it, playerMoves.end());

    /*std::cout << "Possible moves for " << (board.isWhiteTurn() ? "White" : "Black") << ":\n";


    for (const Move &a : playerMoves) {

        std::cout << a.getMovingPiece()->getPieceType() << " from " << a.getOldX() << " " << 7 - a.getOldY() << " to " << a.getNewX() << " " << 7 - a.getNewY() << '\n';
    }
    std::cout << "\n\n\n";*/

    //Checks if the current player is in check
    for (const auto piece : board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces()) {
        if (piece->getPieceType() == Piece::Type::King && dynamic_cast<const King *>(piece)->inCheck(board)) {
            inCheck = true;
        }
    }

    checkGameOver();
}

bool Game::canMove(Move &m) const
{
    const std::vector<Move> &possibleMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;

    for (const Move &move : possibleMoves) {
        if (m == move) {
            //This will add the necessary settings to the move
            if (move.getMoveType() == Move::MoveType::Castle) {
                m = move;
            }
            return true;
        }
    }

    return false;
}

void Game::reset()
{
    board.decipherFen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    gameOver = false;
    heldPiece = nullptr;
    recentPiece = nullptr;
    inCheck = false;
    board.setWhiteTurn(true);
    whiteMoves.clear();
    blackMoves.clear();
    playedMoves.clear();
    playedMoves.reserve(10);
    getMoves();
}

void Game::animateMoves(Node& node)
{
    board.makeMove(node.getValue());
    board.changeTurn();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    draw();
    if (!node.getChildren().empty()) {
        for (auto& a : node.getChildren()) {
            animateMoves(a);
        }
    }
    board.changeTurn();
    board.unmakeMove(node.getValue());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    draw();

}