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
//#include "../usart.h"

//--------Shared Variables---------------------------------
//value of last input
unsigned char lastInput;

//--------Helper Functions---------------------------------
//returns a series of characters, up to a certain length
//custom is a bool to determine whether to use custom characters or not
unsigned int length = 5;
static char answerKey[5];
void getRandKey(int custom)
{
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
			answerKey[i] = allKeypad[rand() % 15];
		}
		answerKey[i] = '\0';
	}
}

//checks to see if a character is the correct one in the sequence
unsigned int anspos = 0;
unsigned char lights = 0x00;
const unsigned char win[] = {'Y', 'o', 'u', ' ', 'W', 'i', 'n', '!', '\0'};
void checkKey(char character)
{
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
				lights = 0x1F;
				LCD_DisplayString(1, win);
				anspos = 0;
		}
	}
	else //incorrect
	{
		anspos = 0;
		lights = 0x00;
	}
	PORTB = lights;
	
}

//--------User defined FSMs--------------------------------
unsigned char Flag;

enum gameStates{gameStart, gameInit, gameWait, gameOut} gamestate;
int gameManager(int state)
{
	unsigned char x;
	
	switch(gamestate)
	{
		case gameStart:
			gamestate = gameInit;
			break;
		case gameInit:
			gamestate = gameOut;
			break;
		case gameWait:
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
	
	if(!Flag)
	{
		switch(gamestate)
		{
			case gameStart:
				break;
			case gameInit:
				//generate random key
				getRandKey(0);
				break;
			case gameWait:
				break;
			case gameOut:
				LCD_ClearScreen();
				LCD_Cursor(1);
				x = GetKeypadKey();
				switch (x) {
					// All 5 LEDs on
					case '\0': PORTB = 0x1F; break;
					// hex equivalent
					case '1': LCD_WriteData('1'); checkKey('1');
					break;
					case '2': LCD_WriteData('2'); checkKey('2');
					break;
					case '3': LCD_WriteData('3'); checkKey('3');
					break;
					case '4': LCD_WriteData('4'); checkKey('4');
					break;
					case '5': LCD_WriteData('5'); checkKey('5');
					break;
					case '6': LCD_WriteData('6'); checkKey('6');
					break;
					case '7': LCD_WriteData('7'); checkKey('7');
					break;
					case '8': LCD_WriteData('8'); checkKey('8');
					break;
					case '9': LCD_WriteData('9'); checkKey('9');
					break;
					case 'A': LCD_WriteData('A'); checkKey('A');
					break;
					case 'B': LCD_WriteData('B'); checkKey('B');
					break;
					case 'C': LCD_WriteData('C'); checkKey('C');
					break;
					case 'D': LCD_WriteData('D'); checkKey('D');
					break;
					case '*': LCD_WriteData('*'); checkKey('*');
					break;
					case '0': LCD_WriteData('0'); checkKey('0');
					break;
					case '#': LCD_WriteData('#'); checkKey('#');
				break;
				// Should never occur. Middle LED off.
				default: PORTB = 0x1B; break;
			}
			Flag = 1;
			break;
		}
	}
	
	return gamestate;
}

enum menusStates{menuStart, startscreen, gameconfigscreen, playscreen, back} menustate;
int menus(int state)
{
	return menustate;
}

// Implement scheduler code from PES.
int main()
{	
	srand(100);
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
	
	//Declare an array of tasks
	static task task1, task2;
	task *tasks[] = {&task1, &task2};
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