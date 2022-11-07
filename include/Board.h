#pragma once
#include <array>
#include "Piece.h"
class Move;

class Board
{
public:
    Board();
    explicit Board(const std::string &fenStr);
    ~Board();
    const Piece* getPiece(unsigned int x, unsigned int y) const;
    void makeMove(const Move &m);
    void unmakeMove(const Move &m);
    const std::vector<const Piece *>& getWhitePieces() const;
    const std::vector<const Piece *>& getBlackPieces() const;
    void decipherFen(const std::string &fen);
private:
    std::array<std::array<Piece *, 8>, 8> b{};
    std::vector<const Piece *> whitePieces;
    std::vector<const Piece *> blackPieces;
    void decipherFenBoard(std::string::const_iterator &it);
    void movePiece(Piece* p, int x, int y);
};
