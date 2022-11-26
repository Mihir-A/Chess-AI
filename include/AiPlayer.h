#pragma once
#include "Board.h"
#include <limits>

class Game;

class AiPlayer
{
public:
    explicit AiPlayer(Board &gameBoard);
    Move getBestMove();
    int evaluatePos() const;
    int evaluatePieceTables(bool white) const;
    int evaluatePiecePos(const Piece* piece) const;
    bool inCheck() const;

private:
    void getMoves();
    int search(int depth, bool isMaximisingPlayer, int alpha, int beta);
    void orderMoves(std::vector<Move> &moves) const;
    static int evaluatePiece(const Piece* piece);
    int evaluatePieces(bool white) const;

    Board &board;
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

    //Values for pieces dependent on location on board taken from
    //https://github.com/SebLague/Chess-AI/blob/d0832f8f1d32ddfb95525d1f1e5b772a367f272e/Assets/Scripts/Core/PieceSquareTable.cs#L4
    static constexpr int pawnPosValues[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr int knightPosValues[] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
    };

    static constexpr int bishopPosValues[] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
    };

    static constexpr int rookPosValues[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
    };

    static constexpr int queenPosValues[] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    };

    static constexpr int kingMiddlePosValues[] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
    };

    static constexpr int kingEndPosValues[] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50
    };
};
