#include "Bishop.h"
#include <iostream>
Bishop::Bishop(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wb.png");

	else
		texture.loadFromFile("assets/piece/bb.png");
}