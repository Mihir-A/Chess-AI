#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
class Board;
class Spot;
class Move;
class Piece
{
protected:
	sf::Texture texture;
	bool white;
public:
	Piece(bool white);
	bool isWhite() const;
	const sf::Texture& getTexture() const;
	virtual bool canMove(const Spot& o, const Spot& n, const Board& b) const;
	virtual void getPossibleMoves(std::vector<Move>& moves) const;
};

