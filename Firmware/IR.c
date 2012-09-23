#include "IR.h"
#include "Timer.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "HardwareProfile.h"

#define SONY_NUM_BITS 7

// Internal variables
unsigned char keyboard_queue[KEY_QUEUE_SIZE];
int keyboard_queue_loc = 0;
int keyboard_queue_ptr = 0;

unsigned char keyboard2_queue[KEY_QUEUE_SIZE];
int keyboard2_queue_loc = 0;
int keyboard2_queue_ptr = 0;

unsigned char keyboard3_queue[KEY_QUEUE_SIZE];
int keyboard3_queue_loc = 0;
int keyboard3_queue_ptr = 0;

unsigned char lastCmd = 0;
short cmdRead = 1;

// Add scancode to the queue for report (which+1)
void AddToQueue(unsigned char scancode, char which);
void RB0Falling(void);
void RB0Rising(void);

void IRIntr_Sony(void)
{
	static char state = 0;
	static char buf = 0;	
	static char bits = 0;
	static int porchIgnore = 0;
	char tmrC = 0;

	// Read the pin
	char IR = PORTBbits.RB0;

	switch(state)
	{
	case 0: // Signal has gone low
		RB0Rising();  // Reverse the polarity of the interrupt
		state = 1;
		TMR0StartReset();
		break;
	case 1: // Signal went back high
		RB0Falling(); // Reverse it again
		TMR0Stop();
		tmrC = TMR0Val(); // Get the timer value out

		if(porchIgnore > 0)
			porchIgnore--;

		if(tmrC < 80 || porchIgnore != 0) // We probably missed the opening porch
			state = 0;
		else
		{
			state = 2;
		}
		break;

	case 2:
		// We're in the signal now.
		// It went low, start the timer
		TMR0StartReset();
		RB0Rising();
		state = 3;
		break;
	case 3:
		// Got a bit
		TMR0Stop();
		tmrC = TMR0Val();
		if(tmrC > 50) // Was probably a 1
			buf = buf * 2 + 1;
		else          // Was probably a 0
			buf *= 2;
		
		bits++;
		RB0Falling();
		if(bits == SONY_NUM_BITS)
		{
			// Done!
			HandleIR(buf);
			porchIgnore = 8;
			buf = 0;
			bits = 0;
			state = 0;
		}
		else
		{
			// Go read another bit.
			state = 2;
		}
	}
}

void HandleIR(char cmd)
{
	switch(cmd)
	{
		case 0x32:  // Vol down - 0b00110010
			//AddToQueue(0x81, 0);
			break;
		case 0x12:  // Vol up - 0b00010010
			//AddToQueue(0x80, 0);
			break; 
		case 0x26:    // Play - 0b010011
			AddToQueue(0xB0, 1);
			break;
		case 0x4E:    // Pause - 0b100111
			AddToQueue(0xB1, 1);
			break;
		case 0x0E:    // Stop - 0b00000111
			AddToQueue(0xB7, 1);
			break;  
		case 0x54:    // Power - 0b00101010
			AddToQueue(0x82, 2);
			break; 
		case 0x06:            // Prev - 0b00000011      
			AddToQueue(0xB6, 1);
			break;
		case 0x46:          // Next - 0b00100011
			AddToQueue(0xB5, 1);
			break;	
	}

	lastCmd = cmd;
	cmdRead = 0;
	
}

short IsCmdReady(void)
{
	return (keyboard_queue_loc != keyboard_queue_ptr);
}

short IsCmd2Ready(void)
{
	return (keyboard2_queue_loc != keyboard2_queue_ptr);
}

short IsCmd3Ready(void)
{
	return (keyboard3_queue_loc != keyboard3_queue_ptr);
}


unsigned char NextCmd(void)
{
	char rv;

	if(!IsCmdReady())
		return 0;

	rv = keyboard_queue[keyboard_queue_loc++];
	
	if(keyboard_queue_loc >= KEY_QUEUE_SIZE)
		keyboard_queue_loc = 0;

	return rv;
}

unsigned char NextCmd2(void)
{
	char rv;

	if(!IsCmd2Ready())
		return 0;

	rv = keyboard2_queue[keyboard2_queue_loc++];
	
	if(keyboard2_queue_loc >= KEY_QUEUE_SIZE)
		keyboard2_queue_loc = 0;

	return rv;
}

unsigned char NextCmd3(void)
{
	char rv;

	if(!IsCmd3Ready())
		return 0;

	rv = keyboard3_queue[keyboard3_queue_loc++];
	
	if(keyboard3_queue_loc >= KEY_QUEUE_SIZE)
		keyboard3_queue_loc = 0;

	return rv;
}

void AddToQueue(unsigned char scancode, char which)
{
	if(which == 0)
	{
		keyboard_queue[keyboard_queue_ptr] = scancode;
		keyboard_queue_ptr++;
		if(keyboard_queue_ptr >= KEY_QUEUE_SIZE)
			keyboard_queue_ptr = 0;
	}
	else if(which == 1)
	{
		keyboard2_queue[keyboard2_queue_ptr] = scancode;
		keyboard2_queue_ptr++;
		if(keyboard2_queue_ptr >= KEY_QUEUE_SIZE)
			keyboard2_queue_ptr = 0;
	}
	else
	{
		keyboard3_queue[keyboard3_queue_ptr] = scancode;
		keyboard3_queue_ptr++;
		if(keyboard3_queue_ptr >= KEY_QUEUE_SIZE)
			keyboard3_queue_ptr = 0;
	}
}

unsigned char GetLastCmd(unsigned char* cmd)
{
	if(cmdRead)
	{
		return 0;
	}
	else
	{
		cmdRead = 1;
		*cmd = lastCmd;
		return 1;
	}
}

void RB0Falling(void)
{
	INTCON2 &= 0b10111111;
}

void RB0Rising(void)
{
	INTCON2 |= 0b01000000;
}