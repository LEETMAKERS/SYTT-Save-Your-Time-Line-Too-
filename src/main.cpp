#include "Game.hpp"
#include <ncurses.h>
#include "Screen.hpp"
#include <unistd.h>

Screen	screen;
int main()
{
	Game	game;

	game.initGameScreen();
	usleep(100000);

	while (true) {
		game.processInput();
		game.updateGameState();
		game.renderGame();
	}
	return 0;
}
