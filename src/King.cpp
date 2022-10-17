#include "King.h"
#include <iostream>
King::King(bool white) 
	: Piece(white) 
{
	if (white) 
		texture.loadFromFile("assets/piece/wk.png");
	
	else 
		texture.loadFromFile("assets/piece/bk.png");
}