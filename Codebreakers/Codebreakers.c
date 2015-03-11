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
//#include "lcd_8bit_task.h"
#include "queue.h"
#include "scheduler.h"
#include "stack.h"
#include "timer.h"
#include "../customchar.h"
//#include "../usart.h"

//--------Global Variables---------------------------------
//for button presses
unsigned char Flag;
//length of answer key
static char answerKey[5];
//length of distortion key;
static char distortionKey[4];
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

//MACROS for custom char
#define SMALLBLOCK 0
#define LARGEBLOCK 1
#define TROPHY 2
#define DIS_ONE 3
#define DIS_TWO 4
#define DIS_THREE 5
#define DIS_FOUR 6
#define DIS_FIVE 7
#define DIS_SIX 8

//--------Display Strings---------------------------------
//display round number
const unsigned char roundDisplay[] = {'R', 'o', 'u', 'n', 'd', '\0'};
//win message
const unsigned char winDisplay[] = {'P', 'l', 'a', 'y', 'e', 'r', ' ', 'O', 'n', 'e', ' ', 'W', 'i', 'n', 's', '\0'};
//start message
const unsigned char openingDisplay[] = {'C', 'o', 'd', 'e', 'b', 'r', 'e', 'a', 'k', 'e', 'r', 's', ' ', ' ', 'R', 'i', 'c', 'a', ' ', 'F', 'e', 'n', 'g', '\0'};
//config message
const unsigned char configDisplay[] = {'H', 'o', 'w', ' ', 'm', 'a', 'n', 'y', ' ', 'r', 'o', 'u', 'n', 'd', 's', '?', '\0'};

//--------Helper Functions---------------------------------
//returns a series of characters, up to a certain length
//custom is a bool to determine whether to use custom characters or not
void getRandKey(int custom)
{
	srand(seedVar);
	char allKeypad[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', '\0'};
	char customKeypad[] = {DIS_ONE, DIS_TWO, DIS_THREE, DIS_FOUR, DIS_FIVE, DIS_SIX};
	//randomly generated key that both players must copy
	int i = 0;
	if (custom)
	{
		for (i = 0; i < 4; ++i)
		{
			distortionKey[i] = customKeypad[rand() % 6];
		}
		distortionKey[i] = '\0';
	}
	else
	{
		for (i = 0; i < 5; ++i)
		{
			//answerKey[i] = allKeypad[rand() % 14];
			answerKey[i] = allKeypad[1];
		}
		answerKey[i] = '\0';
	}
}

//runs at the end of each round
void roundWon()
{
	//display win
	LCD_DisplayString(1, winDisplay);
	LCD_Cursor(23); LCD_WriteData(TROPHY);
	LCD_Cursor(24); LCD_WriteData(TROPHY);
	LCD_Cursor(25); LCD_WriteData(TROPHY);
	sleep(500000);
	LCD_ClearScreen();
					
	//reset lights
	lights = 0x00;
	PORTB = lights;
	
}

//runs at the end of each game
int endGame()
{
	return 1;
}

//checks answer key for correct button press
int checkKey(char character)
{
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
				anspos = 0;
				
				roundWon();
				
				//inc round
				rounds++;
				//if round number is 0, go back to gameconfig
				if (rounds == currentRound + 1)
				{
					status = endGame();
				}
				else
				{
					//get new distortion key
					getRandKey(1);
					//display round
					LCD_DisplayString(1, roundDisplay);
					LCD_Cursor(7);
					LCD_WriteData(rounds + '0');
					sleep(500000);
					LCD_ClearScreen();
				}
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

//sleeps for some time
void sleep(unsigned long x)
{
	while (x > 0)
	{
		x--;
	}
}

//--------User defined FSMs--------------------------------
enum gameStates{gameStart, gameMenu, gameWait, gameOut} gamestate;
int gameManager(int state)
{
	unsigned char x;
	switch(gamestate)
	{
		case gameStart:
			initCustomChar();
			LCD_DisplayString(3, openingDisplay);
			LCD_Cursor(1); LCD_WriteData(SMALLBLOCK);
			LCD_Cursor(2); LCD_WriteData(LARGEBLOCK);
			LCD_Cursor(16); LCD_WriteData(SMALLBLOCK);
			LCD_Cursor(15); LCD_WriteData(LARGEBLOCK);
			sleep(1000000);
			LCD_ClearScreen();
			gamestate = gameMenu;
			break;
		case gameMenu:
			if(playingNow)
			{
				//init game
				LCD_ClearScreen();
				//generate random key and distortion key
				getRandKey(0);
				getRandKey(1);
				//init starting round
				rounds = 1;
				gamestate = gameWait;
				
				//display round number
				LCD_DisplayString(1, roundDisplay);
				LCD_Cursor(7); LCD_WriteData(rounds + '0');
				sleep(500000);
				LCD_ClearScreen();
			}
			break;
		case gameWait:
			//check if game is still playing
			if(!playingNow) gamestate = gameMenu;
			else
			{
				//display distortion
				LCD_Cursor(17); LCD_WriteData(distortionKey[0]);
				LCD_Cursor(18); LCD_WriteData(distortionKey[1]);
				LCD_Cursor(19); LCD_WriteData(distortionKey[2]);
				LCD_Cursor(20); LCD_WriteData(distortionKey[3]);
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
			case gameOut:
				//LCD_ClearScreen();
				LCD_Cursor(1);
				x = GetKeypadKey();
				switch (x) {
					// All 5 LEDs on
					case '\0': break;
					// hex equivalent
					case '1': LCD_WriteData('1'); if(checkKey('1')) playingNow = 0;
					break;
					case '2': LCD_WriteData('2'); if(checkKey('2')) playingNow = 0;
					break;
					case '3': LCD_WriteData('3'); if(checkKey('3')) playingNow = 0;
					break;
					case '4': LCD_WriteData('4'); if(checkKey('4')) playingNow = 0;
					break;
					case '5': LCD_WriteData('5'); if(checkKey('5')) playingNow = 0;
					break;
					case '6': LCD_WriteData('6'); if(checkKey('6')) playingNow = 0;
					break;
					case '7': LCD_WriteData('7'); if(checkKey('7')) playingNow = 0;
					break;
					case '8': LCD_WriteData('8'); if(checkKey('8')) playingNow = 0;
					break;
					case '9': LCD_WriteData('9'); if(checkKey('9')) playingNow = 0;
					break;
					case 'A': LCD_WriteData('A'); if(checkKey('A')) playingNow = 0;
					break;
					case 'B': LCD_WriteData('B'); if(checkKey('B')) playingNow = 0;
					break;
					case 'C': LCD_WriteData('C'); if(checkKey('C')) playingNow = 0;
					break;
					case 'D': LCD_WriteData('D'); if(checkKey('D')) playingNow = 0;
					break;
					case '*': LCD_WriteData('*'); checkKey('*');
					break;
					case '0': LCD_WriteData('0'); if(checkKey('0')) playingNow = 0;
					break;
					case '#': LCD_WriteData('#'); checkKey('#');
				break;
				default:
					break;
			}
			Flag = 1;
			break;
		}
	}
	
	return gamestate;
}

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

int main()
{	
	// Set Data Direction Registers
	// Buttons PORTA[0-7], set AVR PORTA
	// to pull down logic
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xF0; PORTC = 0x0F;
	DDRD = 0xFF; PORTD = 0x00;
	
	LCD_init();
	
	Flag = 0;
	
	// Period for the tasks
	unsigned long int keyPad_period = 1;
	unsigned long int menus_period = 1;
	unsigned long int seeder_period = 1;
	
	//Declare an array of tasks
	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
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