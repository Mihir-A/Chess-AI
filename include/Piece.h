#pragma once
#include "SFML/Graphics.hpp"
#include <vector>
class Board;
class Move;
class Piece
{
protected:
	sf::Texture texture;
	bool white;
	int x;
	int y;
	bool dead;
public:
	Piece(bool white, int x, int y);
	int getX() const;
	int getY() const;
	void moveTo(int xCord, int yCord);
	bool isDead() const;
	void kill();
	bool isWhite() const;
	const sf::Texture& getTexture() const;
	virtual bool canMove(int atemptX, int atemptY, const Board& b) const;
	virtual void getPossibleMoves(std::vector<Move>& moves) const = 0;
};

