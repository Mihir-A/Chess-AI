#include <iostream>
#include "Board.h"
#include "Game.h"
#include "King.h"
#include "SFML/Graphics.hpp"

int main() {
	Game game = Game();
	game.play();
	
	return 0;
}