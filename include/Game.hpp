#pragma once
#include "Player.hpp"
#include <random>
#include "Screen.hpp"

#define MAX_GAME_TIME 20	// Set this according to what seems reasonable for a gameplay loop time for you
#define MAX_SCORE	10
#define MIN_SCORE	1

#define FIRST_PLAYER 0
#define SECOND_PLAYER 1

extern	Screen screen;

class	Game {
	public:
		enum	State {
			START,
			PLAYING,
			END,
		};

	Game(void);
	~Game(void);

	void	chooseRandomPlayer(void);
	void	initGameScreen(void);

	void	processInput(void);
	void	updateGameState(void);
	void	renderGame(void);

	Player	player1() const;
	Player	player2() const;


	private:
		unsigned long	_start_time;
		Player			_players[2];
		std::mt19937	_random;
		bool			_player_turn = FIRST_PLAYER;
		State			_game_state = START;
};
