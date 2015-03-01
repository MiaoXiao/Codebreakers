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
char* getRandKey(int length, int custom)
{
	char allKeypad[15] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', '\0'};
	//randomly generated key that both players must copy
	char key[length];
	int i = 0;
	if (custom)
	{
		
	}
	else
	{
		for (i = 0; key[i] != '\0'; ++i)
		{
			key[i] = allKeypad[rand() % 15];
		}
	}
	return key;
}

//--------User defined FSMs--------------------------------
unsigned char Flag;
enum keyStates{keyStart, keyWait, keyOut} keystate;
int keyPad(int state)
{
	unsigned char x;
	
	switch(keystate)
	{
		case keyStart:
		keystate = keyOut;
		break;
		case keyWait:
		x = GetKeypadKey();
		if(x == '\0')
		{
			keystate = keyWait;
			Flag = 0;
		}
		else
		{
			keystate = keyOut;
		}
		break;
		case keyOut:
		keystate = keyWait;
		break;
	}
	
	if(!Flag)
	{
		switch(keystate)
		{
			case keyStart:
				break;
			case keyWait:
				break;
			case keyOut:
			LCD_Cursor(1);
			x = GetKeypadKey();
			switch (x) {
				// All 5 LEDs on
				case '\0': PORTB = 0x1F; break;
				// hex equivalent
				case '1': LCD_WriteData('1'); lastInput = '1';
				break;
				case '2': LCD_WriteData('2'); lastInput = '2';
				break;
				case '3': LCD_WriteData('3'); lastInput = '3';
				break;
				case '4': LCD_WriteData('4'); lastInput = '4';
				break;
				case '5': LCD_WriteData('5'); lastInput = '5';
				break;
				case '6': LCD_WriteData('6'); lastInput = '6';
				break;
				case '7': LCD_WriteData('7'); lastInput = '7';
				break;
				case '8': LCD_WriteData('8'); lastInput = '8';
				break;
				case '9': LCD_WriteData('9'); lastInput = '9';
				break;
				case 'A': LCD_WriteData('A'); lastInput = 'A';
				break;
				case 'B': LCD_WriteData('B'); lastInput = 'B';
				break;
				case 'C': LCD_WriteData('C'); lastInput = 'C';
				break;
				case 'D': LCD_WriteData('D'); lastInput = 'D';
				break;
				case '*': LCD_WriteData('*'); lastInput = '*';
				break;
				case '0': LCD_WriteData('0'); lastInput = '0';
				break;
				case '#': LCD_WriteData('#'); lastInput = '#';
				break;
				// Should never occur. Middle LED off.
				default: PORTB = 0x1B; break;
			}
			Flag = 1;
			break;
		}
	}
	
	return keystate;
}


enum menusStates{menuStart, startscreen, gameconfigscreen, playscreen, back} menustate;
int menus(int state)
{
	return menustate;
}

// Implement scheduler code from PES.
int main()
{
	//random seed
	//srand(time(NULL));
	
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
	static task task1, task2, task3, task4;
	task *tasks[] = {&task1, &task2};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	
	// Task 1
	task1.state = keyStart;
	task1.period = keyPad_period;
	task1.elapsedTime = keyPad_period;
	task1.TickFct = &keyPad;
	
	//Task 2
	task2.state = menuStart;
	task2.period = menus_period;
	task2.elapsedTime = menus_period;
	task2.TickFct = &menus;
	//Task 3
	
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