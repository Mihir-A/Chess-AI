#include "Pawn.h"
#include <iostream>
Pawn::Pawn(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wp.png");

	else
		texture.loadFromFile("assets/piece/bp.png");
}