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
#include "scheduler.h"
#include "timer.h"
#include "../customchar.h"
#include "../usart.h"

//declare bool type
typedef int bool;
enum { false, true };

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

//number of currentRound
int currentRound = 0;
int totalRounds = 1;
int roundCounter = 1;

//whether game is running or not
bool playingNow = false;

//if player lost game
bool lostGame = false;

//if the end of the round
bool roundEnd = false;

//if greater than 0, loop for a screen
int screenLoop = 0;
//time managing
unsigned int seconds_ones = -1;
unsigned int seconds_tens = 0;
unsigned int minutes = 0;
bool time_exception = false;
//whether time should start ticking
bool timeStarting = false;
#define TIMETRACK 1000
int timeTrack = TIMETRACK;
//change disruption string at this second
int disruption_second = 5;
//whether disruption string has been changed or not
bool disruption_changed = false;
//whether player is immune to powerups or not
bool immune = false;
//time until immune is over
int immune_counter = 0;

bool reverseControls = false;
int reverseControls_time = 0;

//MACROS for custom char
#define BLOCK 6
#define WINLOGO 1
#define LOSELOGO 2
#define DIS_ONE 3
#define DIS_TWO 4
#define DIS_THREE 5
#define DIS_FOUR 6
#define DIS_FIVE 7
#define DIS_SIX 8

//holds possible code
char code[] = {' ', ' ', '\0'};
unsigned int codepos = 0;
char playeronePower[] = {' ', ' ', ' ', '\0'};
int playeroneNumbPowerUp = 0;

unsigned char customKeypad[] = {DIS_ONE, DIS_TWO, DIS_THREE, DIS_FOUR, DIS_FIVE, DIS_SIX};

//--------Display Strings---------------------------------
//display totalRounds number
const unsigned char totalRoundsDisplay[] = {'R', 'o', 'u', 'n', 'd', '\0'};
//win message
const unsigned char winDisplay[] = {'W', 'i', 'n', 'n', 'e', 'r', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'o', 'd', 'e', ':', '\0'};
const unsigned char loseDisplay[] = {'L', 'o', 's', 't', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'C', 'o', 'd', 'e', ':', '\0'};
//start message
const unsigned char openingDisplay[] = {'C', 'o', 'd', 'e', 'b', 'r', 'e', 'a', 'k', 'e', 'r', 's', ' ', ' ', 'R', 'i', 'c', 'a', ' ', 'F', 'e', 'n', 'g', '\0'};
//config message
const unsigned char configDisplay[] = {'P', 'r', 'e', 's', 's', ' ', '0', ' ', 't', 'o', ' ', 's', 't', 'a', 'r', 't', '\0'};

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
void freeze( )
{
	LCD_Cursor(6); LCD_WriteData('F');
	LCD_Cursor(7); LCD_WriteData('R');
	LCD_Cursor(8); LCD_WriteData('O');
	LCD_Cursor(9); LCD_WriteData('Z');
	LCD_Cursor(10); LCD_WriteData('E');
	LCD_Cursor(11); LCD_WriteData('N');
	screenLoop = 1000 * 10;
	LCD_Cursor(33);
	time_exception = true;
}

//get next second to change disruption again
void setNewDisruptionTime()
{
	disruption_changed = false;
	srand(seedVar);
	int newDisruptionSec = (rand() % (20 - 5)) + 5;
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
	LCD_Cursor(2); LCD_WriteData(WINLOGO);
	LCD_Cursor(3); LCD_WriteData(LOSELOGO);
	LCD_Cursor(4); LCD_WriteData(DIS_ONE);
	LCD_Cursor(5); LCD_WriteData(DIS_TWO);
	LCD_Cursor(6); LCD_WriteData(DIS_THREE);
	LCD_Cursor(7); LCD_WriteData(DIS_FOUR);
	LCD_Cursor(8); LCD_WriteData(DIS_FIVE);
	LCD_Cursor(9); LCD_WriteData(DIS_SIX);
	while(1){}
}

//returns a series of characters, up to a certain length
//custom is a bool to determine whether to use custom characters or not
char allKeypad[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '\0'};
void getRandKey(int custom)
{
	srand(seedVar);
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
			answerKey[i] = allKeypad[rand() % 10];
			//answerKey[i] = allKeypad[0];
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

//init all these things each new totalRounds
void beginNewRound()
{	
	reverseControls = false;
	reverseControls_time = 0;
	//get new disruption time
	setNewDisruptionTime();
	
	//reset ans pos
	anspos = 0;
	//reset all powerups
	playeroneNumbPowerUp = 0;
	playeronePower[0] = ' ';
	playeronePower[1] = ' ';
	playeronePower[2] = ' ';
	
	//inc current round
	currentRound++;
	
	//get new distortion key
	getRandKey(1);
	//get new answer key
	getRandKey(0);
	
	//reset time
	minutes = 0;
	seconds_ones = -1;
	seconds_tens = 0;
	//start timer
	timeTrack = TIMETRACK;
	lostGame = false;
	timeStarting = true;
	roundEnd = false;
	
	immune_counter = 0;
}

//checks answer key for correct button press
//returns true if roundover
bool checkKey(char character)
{
	//reset code
	codepos = 0;
	bool status = false;
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
				status = true;
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
			freeze();
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
					if (code[1] == 'A')  //Attack: freeze other player's screen for 5 seconds and reset their code progress
					{
						USART_Send('A', 0);
					}
					else if (code[1] == 'B') //Bug: reverse their entire keyboard for 30 seconds
					{
						USART_Send('B', 0);
					}
					else if (code[1] == 'C') //Counter: player is immune to negative powerups for 20 seconds
					{
						immune = true;
						immune_counter = (20 * 1000);
						LCD_Cursor(5); LCD_WriteData('I');
						LCD_Cursor(6); LCD_WriteData('M');
						LCD_Cursor(7); LCD_WriteData('M');
						LCD_Cursor(8); LCD_WriteData('U');
						LCD_Cursor(9); LCD_WriteData('N');
						LCD_Cursor(10); LCD_WriteData('E');
						LCD_Cursor(11); LCD_WriteData(' ');
					}
					else if (code[1] == 'D') //Disrupt: one digit on the other player's code is permanently changed
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
	LCD_Cursor(33);
}

//--------User defined FSMs--------------------------------
//manage  game
enum gameStates{gameStart, gameMenu, resultsScreen, displayRoundScreen, roundEndScreen, roundTransition, gameWait, gameOut} gamestate;
int gameManager(int state)
{
	//gameStart: start of game
	//gameMenu: in menu now
	//resultsScreen: show resultsScreen of all totalRounds, then go back to menu
	//displayRoundScreen: display totalRounds
	//roundEndScreen: display who win and who lost, and the correct code
	//roundTransition: either go to next round, or end game
	//gameWait: wait for player input
	//gameOut: process input
	if (screenLoop > 0) return gamestate;
	unsigned char x;
	switch(gamestate)
	{
		case gameStart: // switch to game menu
			gamestate = gameMenu;
			break;
		case gameMenu: //wait until player exits menu, then begin game
			if(playingNow)
			{
				//set currentRound to 0 (which will soon be incremented to 1)
				currentRound = 0;
				roundCounter = totalRounds;
				beginNewRound();
				//enable time
				timeStarting = true;
				gamestate = displayRoundScreen;
			}
			break;
		case resultsScreen: //display results of all games
			playingNow = false;
			gamestate = gameMenu;
			break;
		case displayRoundScreen: //  just display distortion before switching to game
			//go to next round
			beginNewRound();
			LCD_ClearScreen();
			displayDistortion();
			//getPowerup();
			gamestate = gameWait;
			break;
		case roundEndScreen: //after displaying who won/lost, find out if you should end the game or not
			gamestate = roundTransition;
			break;
		case roundTransition: //if game over, show results of game. if not gameover, display next round
			//reset lights
			PORTB = 0x00;
			roundCounter--;
			if (1) // end game
			{
				//disable time
				timeStarting = false;
				gamestate = resultsScreen;
			}
			else //go to next round
			{
				//playingNow = true;
				gamestate = displayRoundScreen;
			}
			break;
		case gameWait: //wait for player input, check if game is lost, or if affected by a powerup
			//check if you won
			if (roundEnd) gamestate = roundEndScreen;
			else if (USART_HasReceived(0))
			{
				char temp = USART_Receive(0);
				//check if other player won
				if(temp == 'l')
				{
					gamestate = roundEndScreen;
					lostGame = true;
				}
				else if (!immune) //player is only affected if they are not immune
				{
					if (temp == 'A')
					{
						freeze();
					}
					else if (temp == 'B')
					{	
						reverseControls = true;
						reverseControls_time = (30 * 1000);
						LCD_Cursor(6); LCD_WriteData('B');
						LCD_Cursor(5); LCD_WriteData('U');
						LCD_Cursor(7); LCD_WriteData('G');
						LCD_Cursor(8); LCD_WriteData(' ');
						LCD_Cursor(9); LCD_WriteData(' ');
						LCD_Cursor(10); LCD_WriteData(' ');
						LCD_Cursor(11); LCD_WriteData(' ');
						LCD_Cursor(33);
					}
					else if (temp == 'D')
					{
						int randNum = rand() % 10;
						int randpos = rand() % 5;
						if(allKeypad[randNum] != answerKey[randpos])
						{
							answerKey[randpos] = allKeypad[randNum];
						}
						else
						{
							if (randNum == 0) answerKey[randpos] = allKeypad[1];
							else
							{
								answerKey[randpos] = allKeypad[randNum - 1];
							}
						}
						
					}
				}

				USART_Flush(0);
			}
			else
			{
				//check for input
				x = GetKeypadKey();
				if(x == '\0')
				{
					gamestate = gameWait;
					Flag = false;
				}
				else
				{
					gamestate = gameOut;
				}
			}
			break;
		case gameOut:
			gamestate = gameWait;
			break;
	}

	switch(gamestate)
	{
		case gameStart: //do nothing at start
			break;
		case gameMenu: //do nothing during menu
			break;
		case resultsScreen: //display entire game results
			break;
		case displayRoundScreen: //display currentRound / total totalRounds
			LCD_ClearScreen();
			LCD_DisplayString(1, totalRoundsDisplay);
			LCD_Cursor(7); LCD_WriteData(currentRound + '0');
			LCD_Cursor(8); LCD_WriteData('/');
			LCD_Cursor(9); LCD_WriteData(totalRounds + '0');
			LCD_Cursor(33);
			screenLoop = 3000;
			break;
		case gameWait: //do nothing but wait for next input
			break;
		case roundEndScreen: //display who won or lost, and correct code
			if (!lostGame) // you win!
			{
				LCD_DisplayString(1, winDisplay);
				LCD_Cursor(16); LCD_WriteData(WINLOGO);
			}
			else // you lose!
			{
				LCD_DisplayString(1, loseDisplay);
				LCD_Cursor(16); LCD_WriteData(LOSELOGO);
			}
			//display correct code
			LCD_Cursor(22); LCD_WriteData(answerKey[0]);
			LCD_Cursor(23); LCD_WriteData(answerKey[1]);
			LCD_Cursor(24); LCD_WriteData(answerKey[2]);
			LCD_Cursor(25); LCD_WriteData(answerKey[3]);
			LCD_Cursor(26); LCD_WriteData(answerKey[4]);
			LCD_Cursor(33);
			screenLoop = 5000;
			break;
		case roundTransition:
			//only decide next state to go to
			break;
		case gameOut:
			if(!Flag)
			{
				LCD_Cursor(1);
				x = GetKeypadKey();
				char lastEntered = '\0';
				char codeEntered = '\0';
				switch (x) 
				{
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
					case '5': if (!reverseControls) lastEntered = '5'; else lastEntered = '6';
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
				}
				//display last entered digit
				if (lastEntered != '\0') LCD_WriteData(lastEntered); LCD_Cursor(33);
				//check if last entered if part of key
				if (lastEntered != '\0')
				{
					//check if round is won
					if (checkKey(lastEntered))
					{
						//end the game for the next player
						if (USART_IsSendReady(0))
						{
							USART_Send('l', 0);
						}
						else
						{
							lostGame = false;
						}
						roundEnd = true;
					}
				}
				//check user code
				else if (codeEntered != '\0')
				{
					//if # or *, it should always be included in the code
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
				Flag = true;
				break;
			}
	}
	return gamestate;
}

//manage menu
enum menusStates{menuStart, openingScreen, message, menuConfigWait, playMode, menuConfigOut} menustate;
int menus(int state)
{
	//menuStart : start of menu, init custom characters
	//openingScreen: title screen, should only run once
	//message: display message to screen
	//menuConfigOut: wait for player input and enter in number of totalRoundss.
	//playMode: game in progress
	//menuConfigOut: process input
	if (screenLoop > 0) return menustate;
	unsigned char x;
	switch(menustate)
	{
		case menuStart:
			initCustomChar();
			//debugAllCustoms();
			menustate = openingScreen;
			break;
		case openingScreen:
			menustate = message;
			break;
		case message:
			menustate = menuConfigWait;
			break;
		case menuConfigWait:
			if (USART_HasReceived(0)) //check if other player has started game
			{
				//set number of totalRounds
				totalRounds = USART_Receive(0);
				USART_Flush(0);
				
				//switch to play mode
				playingNow = true;
				menustate = playMode;
			}
			else //look for input
			{
				LCD_Cursor(33);
				x = GetKeypadKey();
				if(x == '\0') //no input entered
				{
					menustate = menuConfigWait; //do nothing
					Flag = false;
				}
				else //input was entered
				{
					menustate = menuConfigOut; //process input
				}
			}
			break;
		case playMode:
			if (!playingNow) //if not playing game anymore, go back to message screen
			{
				 menustate = message;
			}
			break;
		case menuConfigOut:
			menustate = menuConfigWait;
			break;
	}

	switch(menustate)
	{
		case menuStart:
			break;
		case openingScreen: //display opening
			//debugAllCustoms();
			LCD_DisplayString(3, openingDisplay);
			LCD_Cursor(1); LCD_WriteData(BLOCK);
			LCD_Cursor(2); LCD_WriteData(BLOCK);
			LCD_Cursor(16); LCD_WriteData(BLOCK);
			LCD_Cursor(15); LCD_WriteData(BLOCK); LCD_Cursor(33);
			screenLoop = 3000;
			break;
		case message:
			LCD_ClearScreen();
			LCD_DisplayString(1, configDisplay); 
			//LCD_Cursor(17); LCD_WriteData(totalRounds + '0'); LCD_Cursor(33);
			break;
		case menuConfigWait:
			break;
		case playMode:
			break;
		case menuConfigOut:
			if (!Flag)
			{
				LCD_Cursor(17);
				x = GetKeypadKey();
				switch (x) 
				{
					case '\0': break;
					/*
					case '#': //iincrement roundTotal
						if (totalRounds != 9) totalRounds++; LCD_WriteData(totalRounds + '0');
						break;*/
					case '0': //start game
						if (USART_IsSendReady(0)) //start the game for the other player too
						{
							USART_Send(totalRounds, 0);
						}
						playingNow = true;
						LCD_ClearScreen();
						menustate = playMode;
						break;
						/*
					case '*': //decrement roundTotal
						if (totalRounds != 1) totalRounds--; LCD_WriteData(totalRounds + '0');
						break;*/
				}
				Flag = true;
				break;
			}
	}
	return menustate;
}

//seed for random variables
enum seedStates{seedStart, seedUpdate} seedstate;
int seeder(int state)
{
	if (screenLoop > 0) return seedstate;
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

	//check if looping a screen
	if (screenLoop > 1) 
	{
		if (USART_HasReceived(0))
		{
			char temp = USART_Receive(0);
			//check if other player won
			if(temp == 'l')
			{
				gamestate = roundEndScreen;
				lostGame = true;
				screenLoop = 0;
			}
		}
		screenLoop--;
	}
	else if (screenLoop == 1)
	{
		//clear any powerup message
		LCD_Cursor(5); LCD_WriteData(' ');
		LCD_Cursor(6); LCD_WriteData(' ');
		LCD_Cursor(7); LCD_WriteData(' ');
		LCD_Cursor(8); LCD_WriteData(' ');
		LCD_Cursor(9); LCD_WriteData(' ');
		LCD_Cursor(10); LCD_WriteData(' ');
		LCD_Cursor(11); LCD_WriteData(' '); LCD_Cursor(33);
		screenLoop--;
		time_exception = false;
	}
	if (immune)
	{
		reverseControls = false;
		immune_counter--;
		if (immune_counter <= 0)
		{
			immune = false;
			LCD_Cursor(5); LCD_WriteData(' ');
			LCD_Cursor(6); LCD_WriteData(' ');
			LCD_Cursor(7); LCD_WriteData(' ');
			LCD_Cursor(8); LCD_WriteData(' ');
			LCD_Cursor(9); LCD_WriteData(' ');
			LCD_Cursor(10); LCD_WriteData(' ');
			LCD_Cursor(11); LCD_WriteData(' '); LCD_Cursor(33);
		}
	}
	else if (reverseControls)
	{
		reverseControls_time--;
		if (reverseControls_time <= 0) 
		{
			reverseControls = false;
			LCD_Cursor(5); LCD_WriteData(' ');
			LCD_Cursor(6); LCD_WriteData(' ');
			LCD_Cursor(7); LCD_WriteData(' ');
			LCD_Cursor(8); LCD_WriteData(' ');
			LCD_Cursor(9); LCD_WriteData(' ');
			LCD_Cursor(10); LCD_WriteData(' ');
			LCD_Cursor(11); LCD_WriteData(' '); LCD_Cursor(33);
		}
	}
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
			if(timeTrack == TIMETRACK)
			{
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
				//only show time if not looping a screen
				if (screenLoop <= 0 || time_exception)
				{
					LCD_Cursor(29); LCD_WriteData(minutes + '0');
					LCD_Cursor(30); LCD_WriteData(58); //colon
					LCD_Cursor(31); LCD_WriteData(seconds_tens + '0');
					LCD_Cursor(32); LCD_WriteData(seconds_ones + '0');
				}
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
			timeTrack = 0;
			break;
	}
	return timestate;
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
	static task task1, task2, task3, task4;
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