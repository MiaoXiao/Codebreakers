/*
 * Codebreakers.c
 *
 * Created: 24/02/2015 01:04:10
 *  Author: Rica Feng
 */ 

#define F_CPU 8000000

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
//#include "wiringatmega32.h"
//#include "../usart.h"

//--------Helper Functions---------------------------------
/*
byte codeBracket[8] = {
	0x6,0xc,0x18,0x10,0x18,0xc,0x6
};*/


//returns a series of characters, up to a certain length
//custom is a bool to determine whether to use custom characters or not
unsigned int length = 5;
static char answerKey[5];
unsigned int seedVar = 100;
void getRandKey(int custom)
{
	srand(seedVar);
	char allKeypad[15] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', '\0'};
	//randomly generated key that both players must copy
	int i = 0;
	if (custom)
	{
		
	}
	else
	{
		for (i = 0; i < length; ++i)
		{
			//answerKey[i] = allKeypad[rand() % 14];
			answerKey[i] = allKeypad[1];
		}
		answerKey[i] = '\0';
	}
}

//number of rounds
int rounds;
int currentRound = 5;
const unsigned char rounddisplay[] = {'R', 'o', 'u', 'n', 'd', '\0'};
//checks to see if a character is the correct one in the sequence
unsigned int anspos = 0;
//status of lights
unsigned char lights = 0x00;
//win message
const unsigned char win[] = {'P', 'l', 'a', 'y', 'e', 'r', ' ', 'O', 'n', 'e', ' ', 'W', 'i', 'n', 's', '\0'};
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
				anspos = 0;
				
				//display win
				LCD_DisplayString(1, win);
				unsigned long count = 500000;
				while (count >= 1)
				{
					count--;
				}
				LCD_ClearScreen();
				
				//inc round
				rounds++;
				//if round number is 0, go back to gameconfig
				if (rounds == currentRound + 1)
				{
					lights = 0x00;
					PORTB = lights;
					status = 1;
				}
				else
				{
					//display round
					LCD_DisplayString(1, rounddisplay);
					LCD_Cursor(7);
					LCD_WriteData(rounds + '0');
					count = 500000;
					while (count >= 1)
					{
						count--;
					}
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

//--------User defined FSMs--------------------------------
unsigned char Flag;


const unsigned char opening[] = {'C', 'o', 'd', 'e', 'b', 'r', 'e', 'a', 'k', 'e', 'r', 's', '\0'};
const unsigned char signature[] = {'R', 'i', 'c', 'a', ' ', 'F', 'e', 'n', 'g', '\0'};
//whether game is running or not
int playingNow = 0;
enum gameStates{gameStart, gameMenu, gameWait, gameOut} gamestate;
int gameManager(int state)
{
	unsigned char x;
	switch(gamestate)
	{
		case gameStart:
			LCD_DisplayString(1, opening);
			//LCD_DisplayString(17, signature);
			unsigned long count = 1000000;
			gamestate = gameMenu;
			while (count >= 1)
			{
				count--;
			}
			LCD_ClearScreen();
			break;
		case gameMenu:
			if(playingNow)
			{
				//init game
				LCD_ClearScreen();
				//generate random key
				getRandKey(0);
				//init starting round
				rounds = 1;
				gamestate = gameWait;
				
				//display round number
				LCD_DisplayString(1, rounddisplay);
				LCD_Cursor(7);
				LCD_WriteData(rounds + '0');
				unsigned long count = 500000;
				while (count >= 1)
				{
					count--;
				}
				LCD_ClearScreen();
			}
			break;
		case gameWait:
			//check if game is still playing
			if(!playingNow) gamestate = gameMenu;
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
			case gameOut:
				LCD_ClearScreen();
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

const unsigned char config[] = {'H', 'o', 'w', ' ', 'm', 'a', 'n', 'y', ' ', 'r', 'o', 'u', 'n', 'd', 's', '?', '\0'};
enum menusStates{menuStart, gameconfigscreenwait, playscreen, menuOut} menustate;
int menus(int state)
{
	unsigned char x;
	switch(menustate)
	{
		case menuStart:
			LCD_DisplayString(1, config);
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
				LCD_ClearScreen();
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

// Implement scheduler code from PES.
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