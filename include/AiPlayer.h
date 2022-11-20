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
    int evaluatePos() const;
    bool inCheck() const;

private:
    void getMoves();
    int search(int depth, bool isMaximisingPlayer, int alpha, int beta);
    void orderMoves(std::vector<Move> &moves) const;
    static int evaluatePiece(const Piece* piece);
    int evaluatePieces(bool white) const;
    
    Board& board;
    std::vector<Move> whiteMoves;
    std::vector<Move> blackMoves;

    //Values for best move checks

    static constexpr int pawnValue = 100;
    static constexpr int knightValue = 300;
    static constexpr int bishopValue = 320;
    static constexpr int rookValue = 500;
    static constexpr int queenValue = 900;
    static constexpr int negativeInfinity = std::numeric_limits<int>::min();
    static constexpr int positiveInfinity = std::numeric_limits<int>::max();
    static constexpr int capturedPieceValueMultiplier = 10;
};

