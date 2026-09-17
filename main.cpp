#include "Game.hpp"
#include <ncurses.h>
#include "Screen.hpp"

Screen	screen;
int main()
{
	Game	game;

	while (true) {
		game.processInput();
		game.updateGameState();
		game.renderGame();
	}
	return 0;
}
