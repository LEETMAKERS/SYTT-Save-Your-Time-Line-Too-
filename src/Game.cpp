#include <Game.hpp>
#include <Player.hpp>
#include <ctime>
#include <random>
#include <iostream>
#include <ncurses.h>
#include <format>


Game::Game(void) {
	// NOTE : should be millis for the esp
	_start_time = 0;
	_game_state = Game::START;
	_players[FIRST_PLAYER] = Player(FIRST_PLAYER);
	_players[SECOND_PLAYER] = Player(SECOND_PLAYER);
	// NOTE : should be something related to the esp
	_random.seed(time(NULL));
}

Game::~Game(void) {
	// pass
}

// NOTE : there is a builtin random fucntion in arduino
void	Game::chooseRandomPlayer(void) {
	std::uniform_int_distribution<int>	coin(0, 1);

	int	result = coin(_random);
	_player_turn = result;
	screen.drawText(1, 1, (_player_turn == 0 ? "player1 start" : "player2 start"));
}

void	Game::initGameScreen(void) {
	screen.drawRect(screen.getWidth() / 4, 0, screen.getWidth() / 2, screen.getHeight());

	// ▃▃


	

}

void	Game::processInput(void) {
	int	key = getch();

	if (key != ERR) {
		if (key == ' ')
			chooseRandomPlayer();
	}
}

void	Game::updateGameState(void) {

}

void	Game::renderGame(void) {

}

Player	Game::player1() const {
	return _players[0];
}

Player	Game::player2() const {
	return _players[0];
}
