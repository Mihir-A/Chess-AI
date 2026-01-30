#include "AiPlayer.h"
#include "King.h"
#include "Move.h"
#include <algorithm>
#include <iostream>

AiPlayer::AiPlayer(Board &gameBoard)
    : board(gameBoard)
{}

constexpr int searchDepth = 5;
int searched = 0;
int prunned = 0;

Move AiPlayer::getBestMove()
{
    getMoves();
    std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    int bestMove;
    Move bestMoveFound;

    if (!board.isWhiteTurn()) {
        bestMove = negativeInfinity;
        for (Move &move : playerMoves) {
            board.makeMove(move);
            board.changeTurn();
            int value = search(searchDepth, false, negativeInfinity, positiveInfinity);
            board.unmakeMove(move);
            board.changeTurn();
            if (value >= bestMove) {
                bestMove = value;
                bestMoveFound = move;
            }
        }
    }
    else {
        bestMove = positiveInfinity;
        for (Move &move : playerMoves) {
            board.makeMove(move);
            board.changeTurn();
            int value = search(searchDepth, true, negativeInfinity, positiveInfinity);
            board.unmakeMove(move);
            board.changeTurn();
            if (value <= bestMove) {
                bestMove = value;
                bestMoveFound = move;
            }
        }
    }

    //std::cout << "Searched Moves: " << searched << '\n';
    //std::cout << "Prunned Moves: " << prunned << '\n';
    //prunned = 0;
    //searched = 0;

    return bestMoveFound;
}

int AiPlayer::search(int depth, bool isMaximisingPlayer, int alpha, int beta)
{
    //searched++;
    getMoves();
    std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    orderMoves(playerMoves);

    if (playerMoves.empty()) {
        if (inCheck()) {
            return (isMaximisingPlayer ? (negativeInfinity + 1000 - depth) : (positiveInfinity - 1000 + depth));
        }
        return 0;
    }

    if (depth == 0) {
        return -evaluatePos();
    }

    if (isMaximisingPlayer) {
        int bestMove = negativeInfinity;
        for (Move &move : playerMoves) {
            board.makeMove(move);
            board.changeTurn();
            bestMove = std::max(bestMove, search(depth - 1, !isMaximisingPlayer, alpha, beta));
            board.unmakeMove(move);
            board.changeTurn();
            alpha = std::max(alpha, bestMove);
            if (beta <= alpha) {
                prunned++;
                return bestMove;
            }
        }
        return bestMove;
    }
    int bestMove = positiveInfinity;
    for (Move &move : playerMoves) {
        board.makeMove(move);
        board.changeTurn();
        bestMove = std::min(bestMove, search(depth - 1, !isMaximisingPlayer, alpha, beta));
        board.unmakeMove(move);
        board.changeTurn();
        beta = std::min(beta, bestMove);
        if (beta <= alpha) {
            prunned++;
            return bestMove;
        }

    }
    return bestMove;
}

void AiPlayer::orderMoves(std::vector<Move> &moves) const
{
    for (auto &m : moves) {
        if (m.getTargetedPiece()) {
            m.moveValue = capturedPieceValueMultiplier * evaluatePiece(m.getTargetedPiece()) - evaluatePiece(m.getMovingPiece());
        }

        if (m.getMoveType() == Move::MoveType::Promotion) {
            m.moveValue += queenValue;
        }
        //This will make the sorting order correct and eval moves in best to worst order
        //Checking best to worst maximizes pruning
        m.moveValue *= -1;

    }
    std::sort(moves.begin(), moves.end());
}

int AiPlayer::evaluatePiece(const Piece* piece)
{
    int value = 0;
    switch (piece->getPieceType()) {
    case Piece::Type::King:
        value += 0;
        break;
    case Piece::Type::Queen:
        value += queenValue;
        break;
    case Piece::Type::Rook:
        value += rookValue;
        break;
    case Piece::Type::Bishop:
        value += bishopValue;
        break;
    case Piece::Type::Knight:
        value += knightValue;
        break;
    case Piece::Type::Pawn:
        value += pawnValue;
        break;
    }
    return value;
}

int AiPlayer::evaluatePieces(bool white) const
{
    int value = 0;
    const std::vector<const Piece *> &pieces = white ? board.getWhitePieces() : board.getBlackPieces();
    for (const auto piece : pieces) {
        if (!piece->isDead()) {
            value += evaluatePiece(piece);
        }
    }
    return value;
}

int AiPlayer::evaluatePos() const
{
    int whiteEval = 0;
    int blackEval = 0;

    int whiteMaterial = evaluatePieces(true);
    int blackMaterial = evaluatePieces(false);

    whiteEval += whiteMaterial;
    blackEval += blackMaterial;

    whiteEval += evaluatePieceTables(true);
    blackEval += evaluatePieceTables(false);
    int eval = (whiteEval - blackEval);

    return eval;
}

int AiPlayer::evaluatePieceTables(bool white) const
{
    int value = 0;
    const std::vector<const Piece *> &pieces = white ? board.getWhitePieces() : board.getBlackPieces();
    for (const auto piece : pieces) {
        if (!piece->isDead()) {
            value += evaluatePiecePos(piece);
        }
    }
    return value;
}

int AiPlayer::evaluatePiecePos(const Piece* piece) const
{
    int index;

    if (!piece->isWhite()) {
        index = piece->getX() + (7 - piece->getY()) * 8;
    }
    else {
        index = piece->getX() + piece->getY() * 8;
    }

    switch (piece->getPieceType()) {
    case Piece::Type::King:
        if (evaluatePieces(!piece->isWhite()) >= 2 * rookValue + 2 * bishopValue + 3 * pawnValue) {
            return kingMiddlePosValues[index];
        }
        return kingEndPosValues[index];
    case Piece::Type::Queen:
        return queenPosValues[index];
    case Piece::Type::Rook:
        return rookPosValues[index];
    case Piece::Type::Bishop:
        return bishopPosValues[index];
    case Piece::Type::Knight:
        return knightPosValues[index];
    case Piece::Type::Pawn:
        return pawnPosValues[index];
    }
    std::cout << "something went wrong";
    return 0;
}

bool AiPlayer::inCheck() const
{
    for (const auto piece : board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces()) {
        if (piece->getPieceType() == Piece::Type::King && dynamic_cast<const King *>(piece)->inCheck(board)) {
            return true;
        }
    }
    return false;
}

void AiPlayer::getMoves()
{
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
}
