#include "Rook.h"
#include <iostream>
Rook::Rook(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wr.png");

	else
		texture.loadFromFile("assets/piece/br.png");
}