#include "Game.hpp"
#include "Player.hpp"
#include <ctime>
#include <random>
#include <iostream>
#include <ncurses.h>

Game::Game(void) {
	// NOTE : should be millis for the esp
	_start_time = 0;
	_players[0] = Player();
	_players[1] = Player();
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
	mvprintw(0, 0, "%d", _player_turn); // NOTE : to remove 
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
