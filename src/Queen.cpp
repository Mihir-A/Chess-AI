#include "Queen.h"
Queen::Queen(bool white)
	: Piece(white)
{
	if (white)
		texture.loadFromFile("assets/piece/wq.png");

	else
		texture.loadFromFile("assets/piece/bq.png");
}