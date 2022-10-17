#include "King.h"
#include <iostream>
King::King(bool white) 
	: Piece(white) 
{
	if (white) {
		if (!texture.loadFromFile("assets/piece/kingwhite.png")) {
			std::cout << "Unable to load assets/piece/kingwhite.png";
		}
	}
	else {
		if (!texture.loadFromFile("assets/piece/kingblack.png")) {
			std::cout << "Unable to load assets/piece/kingblack.png";
		}
	}
}