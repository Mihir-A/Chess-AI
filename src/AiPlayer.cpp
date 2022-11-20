#include "AiPlayer.h"
#include "King.h"

#include <iostream>

AiPlayer::AiPlayer(Board& gameBoard)
    : board(gameBoard)
{}

constexpr int searchDepth = 4;

Move AiPlayer::getBestMove()
{
    getMoves();
    std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    int bestMove;
    Move bestMoveFound;

    if (!board.isWhiteTurn()) {
        bestMove = negativeInfinity;
        for (Move& move : playerMoves) {
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
    }else {
        bestMove = positiveInfinity;
        for (Move& move : playerMoves) {
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

    return bestMoveFound;
}

void AiPlayer::getMoves()
{
    std::vector<Move>& playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;
    const std::vector<const Piece*>& playerPieces = board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces();

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
            king = dynamic_cast<const King*>(piece);
        }
    }
    int otherC = 0;
    

    //This lambda plays all possible moves and then checks if they cause the king to be in check
    //These moves are removed from the list
    const auto it = std::remove_if(playerMoves.begin(), playerMoves.end(), [this, &king, &otherC](const Move& m)
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


int AiPlayer::search(int depth, bool isMaximisingPlayer, int alpha, int beta)
{
    getMoves();
    std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;

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
        for (Move& move : playerMoves) {
            board.makeMove(move);
            board.changeTurn();
            bestMove = std::max(bestMove, search(depth - 1, !isMaximisingPlayer, alpha, beta));
            board.unmakeMove(move);
            board.changeTurn();
            alpha = std::max(alpha, bestMove);
            if (beta <= alpha) {
                return bestMove;
            }
        }
        return bestMove;
    }
    else {
        int bestMove = positiveInfinity;
        for (Move& move : playerMoves) {
            board.makeMove(move);
            board.changeTurn();
            bestMove = std::min(bestMove, search(depth - 1, !isMaximisingPlayer, alpha, beta));
            board.unmakeMove(move);
            board.changeTurn();
            beta = std::min(beta, bestMove);
            if (beta <= alpha) {
                return bestMove;
            }

        }
        return bestMove;
    }
}

int AiPlayer::evaluatePieces(bool white) const
{
    int value = 0;
    const std::vector<const Piece*>& pieces = white ? board.getWhitePieces() : board.getBlackPieces();
    for (const auto piece : pieces) {
        if (!piece->isDead()) {
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
        }
    }
    return value;
}

int AiPlayer::evaluatePos() {
    int whiteEval = 0;
    int blackEval = 0;

    int whiteMaterial = evaluatePieces(true);
    int blackMaterial = evaluatePieces(false);

   // int whiteMaterialWithoutPawns = whiteMaterial - board.pawns[Board.WhiteIndex].Count * pawnValue;
   // int blackMaterialWithoutPawns = blackMaterial - board.pawns[Board.BlackIndex].Count * pawnValue;
   // float whiteEndgamePhaseWeight = EndgamePhaseWeight(whiteMaterialWithoutPawns);
   // float blackEndgamePhaseWeight = EndgamePhaseWeight(blackMaterialWithoutPawns);

    whiteEval += whiteMaterial;
    blackEval += blackMaterial;
    //whiteEval += MopUpEval(Board.WhiteIndex, Board.BlackIndex, whiteMaterial, blackMaterial, blackEndgamePhaseWeight);
    //blackEval += MopUpEval(Board.BlackIndex, Board.WhiteIndex, blackMaterial, whiteMaterial, whiteEndgamePhaseWeight);

    //whiteEval += EvaluatePieceSquareTables(Board.WhiteIndex, blackEndgamePhaseWeight);
    //blackEval += EvaluatePieceSquareTables(Board.BlackIndex, whiteEndgamePhaseWeight);
    int eval = (whiteEval - blackEval) ;

    return eval;
}

bool AiPlayer::inCheck()
{
    for (const auto piece : board.isWhiteTurn() ? board.getWhitePieces() : board.getBlackPieces()) {
        if (piece->getPieceType() == Piece::Type::King && dynamic_cast<const King*>(piece)->inCheck(board)) {
            return true;
        }
    }
    return false;
}
