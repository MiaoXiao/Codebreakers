/*
 * Codebreakers.c
 *
 * Created: 24/02/2015 01:04:10
 *  Author: Rica Feng
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "../bit.h"
#include "../io.c"
#include "../io.h"
#include "../keypad.h"
//#include "../lcd_8bit_task.h"
#include "../queue.h"
#include "../scheduler.h"
#include "../stack.h"
#include "../timer.h"
//#include "../usart.h"

//--------Shared Variables---------------------------------
unsigned char SM2_output = 0x00;
unsigned char SM3_output = 0x00;
unsigned char pause = 0;

//--------Helper Functions---------------------------------


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
				case '1': LCD_WriteData('1');
				break;
				case '2': LCD_WriteData('2');
				break;
				case '3': LCD_WriteData('3');
				break;
				case '4': LCD_WriteData('4');
				break;
				case '5': LCD_WriteData('5');
				break;
				case '6': LCD_WriteData('6');
				break;
				case '7': LCD_WriteData('7');
				break;
				case '8': LCD_WriteData('8');
				break;
				case '9': LCD_WriteData('9');
				break;
				case 'A': LCD_WriteData('A');
				break;
				case 'B': LCD_WriteData('B');
				break;
				case 'C': LCD_WriteData('C');
				break;
				case 'D': LCD_WriteData('D');
				break;
				case '*': LCD_WriteData('*');
				break;
				case '0': LCD_WriteData('0');
				break;
				case '#': LCD_WriteData('#');
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
	//unsigned long int scrollingText_period = 500;
	
	//Declare an array of tasks
	static task task1;
	task *tasks[] = { &task1};
	const unsigned short numTasks = sizeof(tasks) / sizeof(task*);
	
	// Task 1
	task1.state = keyStart;
	task1.period = keyPad_period;
	task1.elapsedTime = keyPad_period;
	task1.TickFct = &keyPad;
	
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