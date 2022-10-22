#include "Rook.h"
#include <iostream>
Rook::Rook(bool white, int x, int y)
	: Piece(white, x, y)
{
	if (white)
		texture.loadFromFile("assets/piece/wr.png");

	else
		texture.loadFromFile("assets/piece/br.png");
}