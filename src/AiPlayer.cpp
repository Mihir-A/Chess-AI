#include "AiPlayer.h"
#include "King.h"

AiPlayer::AiPlayer(Board& gameBoard)
    : board(gameBoard)
{}

Move AiPlayer::getBestMove()
{
    Move bestMove;

    getMoves();
    std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;

    int bestEval = negativeInfinity;
    for (auto& move : playerMoves) {
        board.makeMove(move);
        board.changeTurn();
        int val = -search(3);
        if (val > bestEval) {
            bestEval = val;
            bestMove = move;
        }
        board.changeTurn();
        board.unmakeMove(move);
    }

    return bestMove;
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

    //This lambda plays all possible moves and then checks if they cause the king to be in check
    //These moves are removed from the list
    const auto it = std::remove_if(playerMoves.begin(), playerMoves.end(), [this, &king](const Move& m)
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


int AiPlayer::search(int depth)
{
    getMoves();
    const std::vector<Move> playerMoves = board.isWhiteTurn() ? whiteMoves : blackMoves;

    if (playerMoves.empty()) {
        if (inCheck()) {
            //prioritizes moves that checkmate fast
            return negativeInfinity - depth;
        }
        return 1;
    }

    if (depth == 0) {
        return evaluatePos();
    }

    int bestEval = negativeInfinity;
    for (auto& move : playerMoves) {
        board.makeMove(move);
        board.changeTurn();
        bestEval = std::max(bestEval, -search(depth - 1));
        board.changeTurn();
        board.unmakeMove(move);
    }
    return bestEval;

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

    return eval * (board.isWhiteTurn() ? 1 : -1);
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
