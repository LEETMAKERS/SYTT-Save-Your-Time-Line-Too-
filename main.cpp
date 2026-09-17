#include "Game.hpp"
#include <ncurses.h>

int main()
{
	initscr();             // Démarre ncurses
	noecho();              // N'affiche pas les touches pressées dans le terminal
	cbreak();              // Désactive le buffer de ligne (réactivité immédiate)
	nodelay(stdscr, TRUE); // RENTRE GETCH() NON-BLOQUANT !
	curs_set(0);

	Game	game;

	while (true) {
		game.processInput();
		game.updateGameState();
		game.renderGame();
	}
	return 0;
}
