
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef STACK_H
#define STACK_H

#include <stdlib.h> // required for malloc

//Stack
//Data Structure to implement stack MAX_SIZE 255
// ** IMPORTANT ** : Global definitions work, but initalization must occur inside main()
typedef struct _Stack
{
	unsigned char* buffer; 		// Stack of characters
	unsigned char top; 			// Stack top index
	unsigned char num_objects;	// Number of objects in stack
	unsigned char capacity;		// Size of the Stack
} *Stack;
////////////////////////////////////////////////////////////////////////////////
//Functionality - Allocates space and initializes a stack
//Parameter: Positive integer for the size of stack (0~255)
//Returns: Reference to a stack
Stack StackInit(unsigned char size)
{
	Stack S 		= malloc (sizeof(struct _Stack));
	S->buffer 		= malloc (size * sizeof(unsigned char));
	S->top 			= 0;
	S->num_objects	= 0;
	S->capacity		= size;
	
	return S;
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Checks if stack is empty
//Parameter: None
//Returns: 1 if empty else 0
unsigned char StackIsEmpty(Stack S) 
{
	return (S->num_objects == 0);
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Checks if stack is full
//Parameter: None
//Returns: 1 if full else 0
unsigned char StackIsFull(Stack S) 
{
	return (S->num_objects == S->capacity);
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the stack
//Parameter: None
//Returns: None
void StackMakeEmpty(Stack S)
{
	S->top 			= 0;
	S->num_objects 	= 0;
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Push a character onto top of stack
//Parameter: Takes in a single unsigned char
//Returns: 1 if full else 0.
unsigned char StackPush(Stack S, unsigned char value) 
{
	if(S->num_objects < S->capacity)	// If stack is not full
	{
		S->buffer[S->top] = value;		// Place data into correct location
		S->num_objects++;				// Increment number of objects
		S->top++;						// Increment top counter
		return 0;						// Return stack is not full (0)
	}
	return 1;							// Else return stack is full (1)
}
////////////////////////////////////////////////////////////////////////////////
//Functionality - Pop a character from top of stack
//Parameter: None
//Returns: Unsigned char from stack else null character
unsigned char StackPop(Stack S) 
{
	if(S->num_objects > 0)				// If stack is not empty
	{
		S->num_objects--;				// Decrement number of objects
		S->top--;						// Decrement top counter
		return S->buffer[S->top];		// Return data
	}
	return '\0';						// Else return null to indicate empty
}

#endif //STACK_H
