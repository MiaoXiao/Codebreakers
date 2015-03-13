/*
 * Codebreakers.c
 *
 * Created: 24/02/2015 01:04:10
 *  Author: Rica Feng
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "bit.h"
#include "io.c"
#include "io.h"
#include "keypad.h"
#include "queue.h"
#include "scheduler.h"
#include "stack.h"
#include "timer.h"
#include "../customchar.h"
#include "../usart.h"

//--------Global Variables---------------------------------
//for button presses
unsigned char Flag;
//length of answer key
static char answerKey[5];
//length of distortion key;
static char disruptionKey[4];
//seed for random number
unsigned int seedVar = 100;

//checks to see if a character is the correct one in the sequence
unsigned int anspos = 0;
//status of lights
unsigned char lights = 0x00;

//number of rounds
int rounds;
int currentRound = 5;

//whether game is running or not
int playingNow = 0;

//if player lost
int lostGame = 0;

//time managing
unsigned int seconds_ones = -1;
unsigned int seconds_tens = 0;
unsigned int minutes = 0;
//whether time should start ticking
int timeStarting = 0;
#define TIMETRACK 1000
int timeTrack = TIMETRACK;
//change disruption string at this second
int disruption_second = 5;
//whether disruption string has been changed or not
int disruption_changed = 0;

//MACROS for custom char
#define BLOCK 0
#define TROPHY 1
#define DIS_ONE 2
#define DIS_TWO 3
#define DIS_THREE 4
#define DIS_FOUR 5
#define DIS_FIVE 6
#define DIS_SIX 7

//holds possible code
char code[] = {' ', ' ', '\0'};
int codepos = 0;
char playeronePower[] = {' ', ' ', ' ', '\0'};
int playeroneNumbPowerUp = 0;

unsigned char customKeypad[] = {DIS_ONE, DIS_TWO, DIS_THREE, DIS_FOUR, DIS_FIVE, DIS_SIX};

//--------Display Strings---------------------------------
//display round number
const unsigned char roundDisplay[] = {'R', 'o', 'u', 'n', 'd', '\0'};
//win message
const unsigned char winDisplay[] = {'W', 'i', 'n', 'n', 'e', 'r', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'o', 'd', 'e', ':', '\0'};
const unsigned char loseDisplay[] = {'L', 'o', 's', 't', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'o', 'd', 'e', ':', '\0'};
//start message
const unsigned char openingDisplay[] = {'C', 'o', 'd', 'e', 'b', 'r', 'e', 'a', 'k', 'e', 'r', 's', ' ', ' ', 'R', 'i', 'c', 'a', ' ', 'F', 'e', 'n', 'g', '\0'};
//config message
const unsigned char configDisplay[] = {'H', 'o', 'w', ' ', 'm', 'a', 'n', 'y', ' ', 'r', 'o', 'u', 'n', 'd', 's', '?', '\0'};
//--------Helper Functions---------------------------------

//sleeps for some time
void sleep(unsigned long x)
{
	while (x > 0)
	{
		x--;
	}
}

//freeze game
void freeze(unsigned long t)
{
	//LCD_DisplayString(6, "FROZEN");
	LCD_Cursor(33);
	sleep(t);
	//LCD_DisplayString(6, "      ");
				
	//redisplay time
	LCD_Cursor(29); LCD_WriteData(minutes + '0');
	LCD_Cursor(30); LCD_WriteData(58); //colon
	LCD_Cursor(31); LCD_WriteData(seconds_tens + '0');
	LCD_Cursor(32); LCD_WriteData(seconds_ones + '0');
}

int reverseControls = 0;
int effectEnd = 0;
//disable user with powerup
void effect()
{
	unsigned char effect;
	effect = USART_Receive(1);
	if (effect == 'A')
	{
		freeze(5000000);
	}
	else if (effect == 'B')
	{
		reverseControls = 1;
		effectEnd = 10;
	}
	else if (effect == 'D')
	{
		anspos = 0;
	}
	
}

//get next second to change disruption again
void setNewDisruptionTime()
{
	disruption_changed = 0;
	srand(seedVar);
	int newDisruptionSec = (rand() % (50 - 5)) + 5;
	disruption_second += newDisruptionSec;
}

//display distortion string
void displayDistortion()
{
	//LCD_Cursor(17); LCD_WriteData(BRACKET);
	LCD_Cursor(17); LCD_WriteData(disruptionKey[0]);
	LCD_Cursor(18); LCD_WriteData(disruptionKey[1]);
	LCD_Cursor(19); LCD_WriteData(disruptionKey[2]);
	LCD_Cursor(20); LCD_WriteData(disruptionKey[3]);
	LCD_Cursor(33);
}

//slightly change disruption key
void changeDisruption()
{
	disruption_changed = 1;
	
	srand(seedVar);
	int randPos = rand() % 4;
	int randDisruptionChar = rand() % 6;
	
	//make sure to change char to a different one
	if (disruptionKey[randPos] != customKeypad[randDisruptionChar])
	{
		disruptionKey[randPos] = customKeypad[randDisruptionChar];
	}
	else
	{
		if (customKeypad[randDisruptionChar + 1] != '\0')
		{
			disruptionKey[randPos] = customKeypad[randDisruptionChar + 1];
		}
		else
		{
			disruptionKey[randPos] = customKeypad[randDisruptionChar - 1];
		}
	}
	
	displayDistortion();
}

//display all custom characters (debug)
void debugAllCustoms()
{
	LCD_Cursor(1); LCD_WriteData(BLOCK);
	LCD_Cursor(2); LCD_WriteData(TROPHY);
	LCD_Cursor(3); LCD_WriteData(DIS_ONE);
	LCD_Cursor(4); LCD_WriteData(DIS_TWO);
	LCD_Cursor(5); LCD_WriteData(DIS_THREE);
	LCD_Cursor(6); LCD_WriteData(DIS_FOUR);
	LCD_Cursor(7); LCD_WriteData(DIS_FIVE);
	LCD_Cursor(8); LCD_WriteData(DIS_SIX);
	while(1){}
}

//returns a series of characters, up to a certain length
//custom is a bool to determine whether to use custom characters or not
void getRandKey(int custom)
{
	srand(seedVar);
	char allKeypad[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
	//randomly generated key that both players must copy
	int i = 0;
	if (custom)
	{
		for (i = 0; i < 4; ++i)
		{
			disruptionKey[i] = customKeypad[rand() % 6];
		}
		disruptionKey[i] = '\0';
	}
	else
	{
		for (i = 0; i < 5; ++i)
		{
			//answerKey[i] = allKeypad[rand() % 10];
			answerKey[i] = allKeypad[1];
		}
		answerKey[i] = '\0';
	}
}

//get a powerup
void getPowerup()
{
	char allPowerUps[] = {'A', 'B', 'C', 'D', '\0'};
	int newpos = 0;
	srand(seedVar);
	//get random powerup
	char powerup = allPowerUps[rand() % 4];
	//find next open space for powerup
	while (newpos <= 2)
	{
		if (playeronePower[newpos] == ' ')
		{
			//get powerup
			playeronePower[newpos] = powerup;
			//update screen (add new powerup)
			LCD_Cursor(14 + newpos); LCD_WriteData(powerup);
			//increase powerup count
			playeroneNumbPowerUp++;
			//exit loop
			newpos = 2;
		}
		newpos++;
	}
}

//runs at the end of each game
int endGame()
{
	timeStarting = 0;
	return 1;
}

//opening screen
void opening()
{
	//OPENING
	initCustomChar();
	//debugAllCustoms();
	LCD_DisplayString(3, openingDisplay);
	LCD_Cursor(1); LCD_WriteData(BLOCK);
	LCD_Cursor(2); LCD_WriteData(BLOCK);
	LCD_Cursor(16); LCD_WriteData(BLOCK);
	LCD_Cursor(15); LCD_WriteData(BLOCK);
	LCD_Cursor(33);
	sleep(1000000);
	LCD_ClearScreen();
}

//init all these things each new round
void newRound()
{	
	setNewDisruptionTime();
	//reset lights
	lights = 0x00;
	PORTB = lights;
	
	//reset ans pos
	anspos = 0;
	//reset all powerups
	playeroneNumbPowerUp = 0;
	playeronePower[0] = ' ';
	playeronePower[1] = ' ';
	playeronePower[2] = ' ';
	
	//inc round
	rounds++;
	
	//get new distortion key
	getRandKey(1);
	//get new puzzle
	getRandKey(0);

	reverseControls = 0;
	effectEnd = 0;
	
	//reset time
	minutes = 0;
	seconds_ones = -1;
	seconds_tens = 0;
	timeTrack = TIMETRACK;
	lostGame = 0;
	getPowerup();
	//display distortion
	displayDistortion();
}

//checks answer key for correct button press
int checkKey(char character)
{
	//reset code
	codepos = 0;
	int status = 0;
	if (answerKey[anspos] == character) //correct
	{
		anspos++;
		switch (anspos)
		{
			case 1:
				lights = 0x01;
				break;
			case 2:
				lights = 0x03;
				break;
			case 3:
				lights = 0x07;
				break;
			case 4:
				lights = 0x0F;
				break;
			case 5:
				lights = 0xFF;
				PORTB = lights;
				
				//end the game for the next player
				if (USART_IsSendReady(0))
				{
					USART_Send('l', 0);
				}
				status = 1;
				break;
		}
	}
	else //incorrect
	{
		anspos = 0;
		lights = 0x00;
	}

	PORTB = lights;
	return status;
}

//checks to see if a valid code, runs corresponding powerup if required
void checkCode()
{
	int newpos = 0;
	if (playeroneNumbPowerUp != 3 && code[0] == '*' && code[1] == '*') //tap in to try to get powerup
	{
		//reset pos
		anspos = 0;
		PORTB = 0x00;
		if (disruption_changed) //right!
		{
			getPowerup();
			setNewDisruptionTime();
		}
		else //wrong!
		{
			freeze(50000);
		}
		
	}
	else if (code[0] == '#' && playeroneNumbPowerUp != 0) //use a powerup that you have
	{	
		while (newpos <= 2) //loop through powerup array and find powerup
		{
			if (playeronePower[newpos] == code[1]) //if this is the right code, use the corresponding powerup
			{
				//remove powerup
				playeronePower[newpos] = ' ';
				//actually run the powerup
				if(USART_IsSendReady(0))
				{
					if (code[1] == 'A')  //Attack: freeze other player's screen for 2 seconds
					{
						USART_Send('A', 0);
					}
					else if (code[1] == 'B') //Bug: reverse other player's controls for 4 seconds
					{
						USART_Send('B', 0);
					}
					else if (code[1] == 'C') //Counter: if the other player uses a powerup (besides another Counter) in the next 4 seconds, the effect is applied on himself
					{
						
					}
					else if (code[1] == 'D') //Disrupt: other player's code progress is reset, and their screen is frozen for 1 second
					{
						USART_Send('D', 0);
					}
				}
				//update screen (remove powerup)
				LCD_Cursor(14 + newpos); LCD_WriteData(' ');
				//remove number of powerups
				playeroneNumbPowerUp--;
				//exit loop
				newpos = 2;
			}
			newpos++;
		}

	}
	
}

//--------User defined FSMs--------------------------------
//manage game states
enum gameStates{gameStart, gameMenu, results, gameWait, gameOut} gamestate;
int gameManager(int state)
{
	unsigned char x;
	switch(gamestate)
	{
		case gameStart:
			LCD_ClearScreen();
			opening();
			gamestate = gameMenu;
			break;
		case gameMenu:
			if(playingNow)
			{
				//start the game for the other player too
				if (USART_IsSendReady(0))
				{
					USART_Send(0xFF, 0);
				}

				//init game
				LCD_ClearScreen();
				//init starting round
				rounds = 1;
				
				//display next round
				LCD_DisplayString(1, roundDisplay);
				LCD_Cursor(7);
				LCD_WriteData(rounds + '0');
				LCD_Cursor(33);
				sleep(1000000);
				LCD_ClearScreen();

				newRound();
				
				//enable time
				timeStarting = 1;
				gamestate = gameWait;
			}
			break;
		case results:
			break;
		case gameWait:
			if (USART_HasReceived(0))
			{
				char temp;
				temp = USART_Receive(0);
				//check if other player won
				if(temp == 'l')
				{
					gamestate = results;
					lostGame = 1;
				}
				USART_Flush(0);
			}
			else if(!playingNow) gamestate = gameMenu; //check if game is still playing
			else
			{	
				x = GetKeypadKey();
				if(x == '\0')
				{
					gamestate = gameWait;
					Flag = 0;
				}
				else
				{
					gamestate = gameOut;
				}
				break;
				case gameOut:
				gamestate = gameWait;
				break;
			}
	}
	
	if(!Flag)
	{
		switch(gamestate)
		{
			case gameStart:
				break;
			case gameMenu:
				//do nothing during menu
				break;
			case gameWait:
				//do nothing but wait for next input
				break;
			case results:
				//display win or loss
				if (!lostGame)
				{
					LCD_DisplayString(1, winDisplay);
					LCD_Cursor(16); LCD_WriteData(TROPHY);
				}
				else
				{
					LCD_DisplayString(1, loseDisplay);
				}
				//display correct code
				LCD_Cursor(22); LCD_WriteData(answerKey[0]);
				LCD_Cursor(23); LCD_WriteData(answerKey[1]);
				LCD_Cursor(24); LCD_WriteData(answerKey[2]);
				LCD_Cursor(25); LCD_WriteData(answerKey[3]);
				LCD_Cursor(26); LCD_WriteData(answerKey[4]);
				LCD_Cursor(33);
				sleep(2000000);
				LCD_ClearScreen();
			
				rounds++;
				//if all rounds finished, go back to gameconfig
				if (rounds == currentRound)
				{
					//end game
					endGame();
					gamestate = gameMenu;
				}
				else
				{
					//display next round
					LCD_DisplayString(1, roundDisplay);
					LCD_Cursor(7);
					LCD_WriteData(rounds + '0');
					LCD_Cursor(33);
					sleep(1000000);
					LCD_ClearScreen();
					//continueplaying
					newRound();
					playingNow = 1;
					gamestate = gameWait;
				}
				break;
			case gameOut:
				//LCD_ClearScreen();
				LCD_Cursor(1);
				x = GetKeypadKey();
				char lastEntered = '\0';
				char codeEntered = '\0';
				switch (x) {
					// All 5 LEDs on
					case '\0': break;
					// hex equivalent
					case '1':  if (!reverseControls) lastEntered = '1'; else lastEntered = '2';
					break;
					case '2':  if (!reverseControls) lastEntered = '2'; else lastEntered = '1';
					break;
					case '3':  if (!reverseControls) lastEntered = '3'; else lastEntered = '4';
					break;
					case '4':  if (!reverseControls) lastEntered = '4'; else lastEntered = '3';
					break;
					case '5': ; if (!reverseControls) lastEntered = '5'; else lastEntered = '6';
					break;
					case '6':  if (!reverseControls) lastEntered = '6'; else lastEntered = '5';
					break;
					case '7':  if (!reverseControls) lastEntered = '7'; else lastEntered = '8';
					break;
					case '8':  if (!reverseControls) lastEntered = '8'; else lastEntered = '7';
					break;
					case '9':  if (!reverseControls) lastEntered = '9'; else lastEntered = '0';
					break;
					case 'A': LCD_WriteData('A'); codeEntered = 'A';
					break;
					case 'B': LCD_WriteData('B'); codeEntered = 'B';
					break;
					case 'C': LCD_WriteData('C'); codeEntered = 'C';
					break;
					case 'D': LCD_WriteData('D'); codeEntered = 'D';
					break;
					case '*': LCD_WriteData('*'); codeEntered = '*';
					break;
					case '0':  if (!reverseControls) lastEntered = '0'; else lastEntered = '9';
					break;
					case '#': LCD_WriteData('#'); codeEntered = '#';
				break;
				default:
					break;
			}
			//display last entered
			if (lastEntered != '\0') LCD_WriteData(lastEntered);
			//check if last entered if part of the combo
			if(lastEntered != '\0')
			{
				//check if next round
				if (checkKey(lastEntered) == 1)
				{
					gamestate = results;
				}
			}
			//check user code
			else if (codeEntered != '\0')
			{
				if (codeEntered == '#') codepos = 0;
				else if (codeEntered == '*' && code[0] != '*') codepos = 0;
				//save last code
				code[codepos] = codeEntered;
				codepos++;
				if (codepos == 2)
				{
					checkCode();
					codepos = 0;
				}					
				
			}
			LCD_Cursor(33);
			Flag = 1;
			break;
		}
	}
	
	return gamestate;
}

//manage menu
enum menusStates{menuStart, gameconfigscreenwait, playscreen, menuOut} menustate;
int menus(int state)
{
	unsigned char x;
	switch(menustate)
	{
		case menuStart:
			LCD_DisplayString(1, configDisplay);
			LCD_Cursor(17);
			LCD_WriteData(currentRound + '0');
			menustate = gameconfigscreenwait;
			break;
		case gameconfigscreenwait:
			//check if other player has started game
			if (USART_HasReceived(0))
			{
				playingNow = 1;
				USART_Flush(0);
			}
			
			LCD_Cursor(33);
			x = GetKeypadKey();
			if(x == '\0')
			{
				menustate = gameconfigscreenwait;
				Flag = 0;
			}
			else
			{
				menustate = menuOut;
			}
			break;
		case playscreen:
			if (!playingNow)
			{
				 menustate = menuStart;
			}
			break;
		case menuOut:
			menustate = gameconfigscreenwait;
			break;
	}
	if (!Flag)
	{
		switch(menustate)
		{
			case menuStart:
				break;
			case gameconfigscreenwait:
				break;
			case playscreen:
				break;
			case menuOut:
			//LCD_ClearScreen();
			LCD_Cursor(17);
			x = GetKeypadKey();
			switch (x) {
				case '\0': break;
				case '*': //inc round numb
				if (currentRound != 1) currentRound--; LCD_WriteData(currentRound + '0');
				break;
				case '0': //game is now starting
				playingNow = 1;
				//LCD_ClearScreen();
				menustate = playscreen;
				break;
				case '#': //dec round numb
				if (currentRound != 9) currentRound++; LCD_WriteData(currentRound + '0');
				break;
				default:
					break;
			}
		Flag = 1;
		break;
		}
	}
	return menustate;
}

//seed for random variables
enum seedStates{seedStart, seedUpdate} seedstate;
int seeder(int state)
{
	switch(seedstate)
	{
		case seedStart:
		seedstate = seedUpdate;
		break;
		case seedUpdate:
		break;
	}
	
	switch(seedstate)
	{
		case seedStart:
		break;
		case seedUpdate:
		if (seedVar <= 60000)
		{
			seedVar++;
		}
		else
		{
			seedVar = 100;
		}
		break;
	}
	return seedstate;
}

//timer
enum timeStates{timeStart, timeWait, timeTick, timeRestart} timestate;
int timer(int state)
{
	switch(timestate)
	{
		case timeStart:
		timestate = timeWait;
		break;
		case timeWait:
		if (timeStarting) timestate = timeTick; 
		break;
		case timeTick:
		if (!timeStarting) timestate = timeRestart;
		break;
		case timeRestart:
		timestate = timeWait;
		break;
	}
	switch(timestate)
	{
		case timeStart:
		break;
		case timeWait:
		break;
		case timeTick:
		if (timeTrack == TIMETRACK)
		{
			//check to see if negative effect wore off
			if(effectEnd <= 0)
			{
				effectEnd--;
			}
			else
			{
				effectEnd = 0;
				reverseControls = 0;
			}
			//check if it is time to change disruption string
			if ((seconds_ones + (seconds_tens * 10)) == disruption_second)
			{
				changeDisruption();
			}
			
			timeTrack = 0;
			seconds_ones++;
			if (seconds_ones == 10)
			{
				seconds_ones = 0;
				seconds_tens++;
				if (seconds_tens == 6)
				{
					seconds_tens = 0;
					minutes++;
					if (minutes == 10)
					{
						minutes = 0;
					}
				}
			}
		
			LCD_Cursor(29); LCD_WriteData(minutes + '0');
			LCD_Cursor(30); LCD_WriteData(58); //colon
			LCD_Cursor(31); LCD_WriteData(seconds_tens + '0');
			LCD_Cursor(32); LCD_WriteData(seconds_ones + '0');
		}
		else
		{
			timeTrack++;
		}
		break;
		case timeRestart:
		seconds_ones = -1;
		seconds_tens = 0;
		minutes = 0;
		timeTrack = TIMETRACK;
		break;
	}
	return timestate;
}

unsigned char resetCode[] = {'#', '#', '#', '#', '#', '0', '4', '1', '5', '\0'};
int resetpos = 0;
enum resetStates{resetStart, resetWait, actualReset, resetOut} resetstate;
int hardReset(int state)
{
	unsigned char x;
	switch(resetstate)
	{
		case resetStart:
		resetpos = 0;
		resetstate = resetWait;
		break;
		case resetWait:
			x = GetKeypadKey();
			if (resetCode[resetpos] == '\0')
			{
				resetstate = actualReset;
			}
			else if (x == '\0')
			{
				resetstate = resetWait;
				Flag = 0;
			}
			else
			{
				resetstate = resetOut;
			}
			break;
		case actualReset:
		resetstate = resetStart;
		break;
		case resetOut:
		resetstate = resetWait;
		break;

	}
	if (!Flag)
	{
		switch(resetstate)
		{
			case resetStart:
			break;
			case resetWait:
			break;
			case actualReset: //reset game
			playingNow = 0;
			gamestate = gameStart;
			LCD_Cursor(1); LCD_WriteData('Y');
			break;
			case resetOut:
			x = GetKeypadKey();
			switch (x) {
				case '\0': resetpos = 0; break;
				case '0':
				if (resetpos == 5) resetpos++;
				else resetpos = 0;
				break;
				case '1':
				if (resetpos == 7) resetpos++;
				else resetpos = 0;
				break;
				case '4':
				if (resetpos == 6) resetpos++;
				else resetpos = 0;
				break;
				case '5':
				if (resetpos == 8) resetpos++;
				else resetpos = 0;
				break;
				case '#':
				if (resetpos < 5) resetpos++;
				else resetpos = 0;
				break;
				default:
				break;
			}
			Flag = 1;
			break;
		}
	}
	return resetstate;
}

/*
enum timeStates{timeStart, timeTick, timeRestart} timestate;
int timer(int state)
{
	switch(timestate)
	{
		
	}
	switch(timestate)
	{
		
	}
	return timestate;
}*/

int main()
{	
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA
	// to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xF0; PORTD = 0x0F;
	
	LCD_init();
	initUSART(0);
	Flag = 0;
	
	// Period for the tasks
	unsigned long int keyPad_period = 1;
	unsigned long int menus_period = 1;
	unsigned long int seeder_period = 250;
	unsigned long int time_period = 1;
	unsigned long int hardReset_period = 1;
	
	//Declare an array of tasks
	static task task1, task2, task3, task4, task5;
	task *tasks[] = {&task1, &task2, &task3, &task4};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	
	// Task 1
	task1.state = gameStart;
	task1.period = keyPad_period;
	task1.elapsedTime = keyPad_period;
	task1.TickFct = &gameManager;
	
	//Task 2
	task2.state = menuStart;
	task2.period = menus_period;
	task2.elapsedTime = menus_period;
	task2.TickFct = &menus;
	
	//Task 3
	task3.state = seedStart;
	task3.period = seeder_period;
	task3.elapsedTime = seeder_period;
	task3.TickFct = &seeder;
	
	//Task 4
	task4.state = timeStart;
	task4.period = time_period;
	task4.elapsedTime = time_period;
	task4.TickFct = &timer;
	
	//Task 5
	task5.state = resetStart;
	task5.period = hardReset_period;
	task5.elapsedTime = hardReset_period;
	task5.TickFct = &hardReset;
	
	// Set the timer and turn it on
	TimerSet(1);
	TimerOn();
	
	// Scheduler for-loop iterator
	unsigned short i;
	while(1) {
		// Scheduler code
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
	// Error: Program should not exit!
	return 0;
}