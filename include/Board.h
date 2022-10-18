#pragma once
#include <vector>
#include <array>
#include "Spot.h"
class Board
{
private:
	std::array<std::array<Spot, 8>, 8> b;
public:
	Board();
	const Spot& getSpot(int x, int y) const;
	void setPiece(int x, int y, Piece* p);
};

