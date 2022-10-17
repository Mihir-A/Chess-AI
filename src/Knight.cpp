#include "Knight.h"
Knight::Knight(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wn.png");

	else
		texture.loadFromFile("assets/piece/bn.png");
}