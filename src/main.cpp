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
#define BUTTON_DEFLECT_2_0 34

#define MAX_GAME_TIME 20	// Set this according to what seems reasonable for a gameplay loop time for you
#define MAX_SCORE	10
#define MIN_SCORE	1

MD_Parola ledMatrix = MD_Parola(HARDWARE_TYPE, DI_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

unsigned long start_time;

int player1_score = 0;
int player2_score = 0;

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
	start_time = millis();
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
// just a simple packman experimentation here nothing much
int frames[6][8*8] = {
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,0,0,0,0,
    0,1,1,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,1,1,0,0,0,0,0,
    0,0,0,1,1,0,0,0
  },
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,0,0,0,0,
    0,1,1,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,
    0,1,0,0,0,0,0,0,
    0,1,1,0,0,0,0,0,
    0,0,0,1,1,0,0,0
  },
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,1,0,0,0,
    0,1,1,1,0,0,0,0,
    1,1,1,0,0,0,0,0,
    1,1,1,0,0,0,0,0,
    0,1,1,0,0,0,0,0,
    0,1,1,1,0,0,0,0,
    0,0,0,1,1,0,0,0
  },
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,1,1,0,0,
    0,1,1,1,1,0,0,0,
    1,1,1,1,0,0,0,0,
    1,1,1,1,0,0,0,0,
    0,1,1,1,0,0,0,0,
    0,1,1,1,1,1,0,0,
    0,0,0,1,1,0,0,0
  },
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,0,0,
    1,1,1,1,1,0,0,0,
    1,1,1,1,0,0,0,0,
    0,1,1,1,1,1,0,0,
    0,1,1,1,1,1,1,0,
    0,0,0,1,1,0,0,0
  },
  {
    0,0,0,1,1,0,0,0,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,
    0,1,1,1,1,1,1,0,
    0,1,1,1,1,1,1,0,
    0,0,0,1,1,0,0,0
  }
};

// coordinates of the character
int y = 0;
int x = 0;

directions dir = STATIONARY;
game_state state = START;

void game_playing(MD_MAX72XX* mx)
{
	if (digitalRead(BUTTON_RIGHT) == LOW || digitalRead(BUTTON_LEFT_2) == LOW)
	{
		dir = RIGHT;
		Serial.println("Right");
	}
	if (digitalRead(BUTTON_LEFT) == LOW || digitalRead(BUTTON_RIGHT_2) == LOW)
	{
		dir = LEFT;
		Serial.println("Left");
	}
	if (digitalRead(BUTTON_DEFLECT_1_0) == LOW && x%8 >= 0 && x%8 <= 4)
	{
		dir = DOWN;
		Serial.println("Up");
	}
	if (digitalRead(BUTTON_DEFLECT_1) == LOW && x%8 > 4 && x%8 <= 8)
	{
		dir = DOWN;
		Serial.println("Up");
	}
	// Serial.println("Deflect 2");
	// delay(1000);
	// Serial.println(digitalRead(BUTTON_DEFLECT_2));
	// delay(1000);
	// Serial.println("Deflect 2_0");
	// delay(1000);
	// Serial.println(digitalRead(BUTTON_DEFLECT_2_0));
	// delay(1000);
	// Serial.println("right 2");
	// delay(1000);
	// Serial.println(digitalRead(BUTTON_RIGHT_2));
	// delay(1000);
	// Serial.println("left 2");
	// delay(1000);
	// Serial.println(digitalRead(BUTTON_LEFT_2));
	// delay(1000);

	if (digitalRead(BUTTON_DEFLECT_2) == LOW && x%8 >= 0 && x%8 <= 4)
	{
		dir = UP;
		Serial.println("Up");
	}
	// if (digitalRead(BUTTON_DEFLECT_2_0) == LOW && x%8 > 4 && x%8 <= 8)
	// {
	// 	dir = UP;
	// 	Serial.println("Up");
	// }

	mx->setPoint(x%8, y%32, true);
	mx->setPoint((x+1)%8, y%32, true);
	mx->setPoint((x+1)%8, (y+1)%32, true);
	mx->setPoint((x)%8, (y+1)%32, true);

	// the capping of the x value will be changed to 16 after getting another MAX72XX module
	if (x <= 0) x = 8;
	if (y <= 0) y = 32;

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
}

unsigned long current_time;

void loop() {
	MD_MAX72XX* mx = ledMatrix.getGraphicObject();

	switch (state)
	{
		case START:
		case FINISH:
			if (digitalRead(BUTTON_RIGHT) == LOW || digitalRead(BUTTON_LEFT) == LOW 
			|| digitalRead(BUTTON_DEFLECT_1) == LOW || digitalRead(BUTTON_DEFLECT_2) == LOW
			|| digitalRead(BUTTON_DEFLECT_1_0) == LOW)
				state = PLAYING;
			ledMatrix.setTextAlignment(PA_CENTER);
			// ledMatrix.print(state == FINISH ? "FINISH" : "START");
			for (int i = 0; i < 8 * 8; i++)
			{
				mx->setPoint((i)%8, i/8 + 12, arrow[i]);
			}
			break;
		case PLAYING:
			current_time = millis();
			// if ((current_time - start_time)/1000 > MAX_GAME_TIME)
			// {
			// 	start_time = millis();
			// 	state = FINISH;
			// }
			// the game-play loop happens here
			game_playing(mx);
			break;
		default:
			break;
	}
	delay(200);
	mx->clear(); 
}
