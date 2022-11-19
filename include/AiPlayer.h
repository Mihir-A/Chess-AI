#pragma once
#include "Board.h"
#include "Node.h"
#include <limits>

class Game;

class AiPlayer
{
public:
    explicit AiPlayer(Board& gameBoard);
    Move getBestMove();
    int evaluatePos();
    bool inCheck();

private:
    void getMoves();
    int search(int depth);
    int evaluatePieces(bool white) const;
    
    Board& board;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;

    //Values for best move checks

    const int pawnValue = 100;
    const int knightValue = 300;
    const int bishopValue = 320;
    const int rookValue = 500;
    const int queenValue = 900;
    const int negativeInfinity = std::numeric_limits<int>::min() + 1000;
    const int positiveInfinity = std::numeric_limits<int>::max();
};

