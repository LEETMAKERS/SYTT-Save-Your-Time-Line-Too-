#include <Arduino.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW

#define MAX_DEVICES 4

#define CS_PIN 21
#define DI_PIN 22
#define CLK_PIN 18

// Left and right buttons for player 1
#define BUTTON_RIGHT 25
#define BUTTON_LEFT 26
// Left and right buttons for player 2
#define BUTTON_RIGHT_2 19
#define BUTTON_LEFT_2 15
// Launch/deflect button for player 1
#define BUTTON_DEFLECT_1 32
#define BUTTON_DEFLECT_1_0 27
// need to define 2 other pins for 2 other buttons
// Launch/deflect button for player 2
// same thing here
#define BUTTON_DEFLECT_2 23
#define BUTTON_DEFLECT_2_0 4

#define MAX_GAME_TIME 60	// Set this according to what seems reasonable for a gameplay loop time for you
#define MAX_SCORE	10
#define MIN_SCORE	1

#define DEFLECT_THREASHOLD 4

MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, DI_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

unsigned long start_time;

int player1_score = 0;
int player2_score = 0;

unsigned long game_speed = 80;
unsigned long max_game_speed = 40;

enum game_state
{
	START,
	PLAYING,
	FINISH
};

// use this enum to set the player turn based on which buttons side was pressed first, or you can 
// assign the turn randomly after a given time, you are free to tackle this however you see fit
enum players_turn
{
	PLAYER1,
	PLAYER2
};

enum directions
{
  STATIONARY,	// set only at the beginning of the game
  UP,
  DOWN,
  RIGHT,
  LEFT
};

void setup() 
{
	pinMode(BUTTON_RIGHT, INPUT_PULLUP);
	pinMode(BUTTON_LEFT, INPUT_PULLUP);
	pinMode(BUTTON_RIGHT_2, INPUT_PULLUP);
	pinMode(BUTTON_LEFT_2, INPUT_PULLUP);
	pinMode(BUTTON_DEFLECT_2, INPUT_PULLUP);
	pinMode(BUTTON_DEFLECT_2_0, INPUT_PULLUP);
	pinMode(BUTTON_DEFLECT_1, INPUT_PULLUP);
	pinMode(BUTTON_DEFLECT_1_0, INPUT_PULLUP);

	ledMatrix.begin();
	ledMatrix.setIntensity(0);
	ledMatrix.displayClear();

	Serial.begin(9600);
}

int arrow[8*8] = {
  0,0,0,0,0,0,0,0,
  0,0,1,1,0,0,0,0,
  0,0,1,1,1,0,0,0,
  0,0,1,1,1,1,0,0,
  0,0,1,1,1,1,1,0,
  0,0,1,1,1,1,0,0,
  0,0,1,1,0,0,0,0,
  0,0,0,0,0,0,0,0
};

int key_frame = 0;

// coordinates of the character
int y = 30;
int x = 0;

directions dir = STATIONARY;
game_state state = START;
players_turn turn = PLAYER1;

void game_playing(MD_MAX72XX* mx)
{
	if (y <= 16) turn = PLAYER2;
	if (y > 16) turn = PLAYER1;
	if ((digitalRead(BUTTON_RIGHT) == LOW && y > 32 - DEFLECT_THREASHOLD - 1) || (digitalRead(BUTTON_LEFT_2) == LOW && y < DEFLECT_THREASHOLD - 1))
	{
		dir = RIGHT;
		Serial.println("Right");
	}
	if ((digitalRead(BUTTON_LEFT) == LOW && y > 32 - DEFLECT_THREASHOLD - 1) || (digitalRead(BUTTON_RIGHT_2) == LOW && y < DEFLECT_THREASHOLD - 1))
	{
		dir = LEFT;
		Serial.println("Left");
	}

	if (digitalRead(BUTTON_DEFLECT_1_0) == LOW  && digitalRead(BUTTON_DEFLECT_1) == HIGH && x%8 >= 0 && x%8 < 4)
	{
		if (turn == PLAYER1 && y > 32 - DEFLECT_THREASHOLD - 1)
		{
			dir = DOWN;
		}
	}
	if (digitalRead(BUTTON_DEFLECT_1) == LOW && digitalRead(BUTTON_DEFLECT_1_0) == HIGH && x%8 >= 4 && x%8 <= 8)
	{
		if (turn == PLAYER1 && y > 32 - DEFLECT_THREASHOLD - 1)
		{
			dir = DOWN;
		}
	}
	if (digitalRead(BUTTON_DEFLECT_2) == LOW && digitalRead(BUTTON_DEFLECT_2_0) == HIGH && x%8 >= 0 && x%8 < 4)
	{
		if (turn == PLAYER2 && y < DEFLECT_THREASHOLD - 1)
		{
			dir = UP;
		}
	}
	if (digitalRead(BUTTON_DEFLECT_2_0) == LOW && digitalRead(BUTTON_DEFLECT_2) == HIGH && x%8 >= 4 && x%8 <= 8)
	{
		if (turn == PLAYER2 && y < DEFLECT_THREASHOLD - 1)
		{
			dir = UP;
		}
	}
	
	mx->setPoint(x, y, true);
	mx->setPoint((x+1), y, true);
	mx->setPoint((x+1), (y+1), true);
	mx->setPoint((x), (y+1), true);

	if (y > 32)
	{
		player2_score++;
		dir = STATIONARY;
		y = 0;
		turn = PLAYER2;
	}
	if (y < 0)
	{
		dir = STATIONARY;
		player1_score++;
		y = 30;
		turn = PLAYER1;
	}

	switch (dir)
	{
		case UP:
			y++;
			break;
		case DOWN:
			y--;
			break;
		case RIGHT:
			x++;
			break;
		case LEFT:
			x--;
			break;
		default:
			break;
	}

	if (x >= 7) x = 0;
	if (x < 0) x = 6;
}

unsigned long current_time;

unsigned long last_frame_time = 0;

int score_drawn = 0;

float subtract_interval = 0.0;

void set_players(int player1_buttons, int player2_buttons)
{
	if (player1_buttons || player2_buttons)
	{
		start_time = millis();
		state = PLAYING;
		if (digitalRead(BUTTON_DEFLECT_1) == LOW)	y = 30;
		if (digitalRead(BUTTON_DEFLECT_1_0) == LOW)	y = 30;
		if (digitalRead(BUTTON_RIGHT) == LOW)	y = 30;
		if (digitalRead(BUTTON_LEFT) == LOW)	y = 30;

		if (digitalRead(BUTTON_RIGHT_2) == LOW)	y = 0;
		if (digitalRead(BUTTON_LEFT_2) == LOW)	y = 0;
		if (digitalRead(BUTTON_DEFLECT_2) == LOW)	y = 0;
		if (digitalRead(BUTTON_DEFLECT_2_0) == LOW)	y = 0;

		if (y == 30) turn = PLAYER1;
		if (y == 0) turn = PLAYER2;

		player1_score = 0;
		player2_score = 0;
	}
}


void loop() 
{
	// Only run a frame once game_speed ms have passed
	unsigned long now = millis();
	int player1_buttons = digitalRead(BUTTON_DEFLECT_1) == LOW || digitalRead(BUTTON_DEFLECT_1_0) == LOW || digitalRead(BUTTON_RIGHT) == LOW || digitalRead(BUTTON_LEFT) == LOW;
	int player2_buttons = digitalRead(BUTTON_RIGHT_2) == LOW || digitalRead(BUTTON_LEFT_2) == LOW || digitalRead(BUTTON_DEFLECT_2) == LOW || digitalRead(BUTTON_DEFLECT_2_0) == LOW;
	if (now - last_frame_time < game_speed)
		return;
	last_frame_time = now;

	MD_MAX72XX* mx = ledMatrix.getGraphicObject();

	if (state != FINISH)
		mx->clear();

	// mx->setPoint(0, 0, true);
	// mx->setPoint(6, 31, true);
	// mx->setPoint(7, 31, true);
	switch (state)
	{
		case START:
			set_players(player1_buttons, player2_buttons);
			for (int i = 0; i < 8 * 8; i++)
			{
				mx->setPoint((i)%8, i/8 + 12, arrow[i]);
			}
			break;
		case FINISH:
			// set_players(player1_buttons, player2_buttons);
			char scoreBuffer[16]; 
			sprintf(scoreBuffer, "%d - %d", player1_score, player2_score);

			ledMatrix.setTextAlignment(PA_CENTER);
			ledMatrix.print(scoreBuffer);
			break;
		case PLAYING:
			subtract_interval += 0.1;

			if (subtract_interval >= 1.0)
			{
				subtract_interval = 0.0;
				if (game_speed > max_game_speed)
					game_speed--;
			}

			current_time = millis();
			if ((current_time - start_time)/1000 > MAX_GAME_TIME)
			{
				start_time = millis();
				state = FINISH;
			}
			// the game-play loop happens here
			game_playing(mx);
			break;
		default:
			break;
	}
}
