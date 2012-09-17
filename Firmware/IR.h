#ifndef __IR_H__
#define __IR_H__

#define KEY_QUEUE_SIZE 50

// Called by the interrupt handler when the state of the IR
// input pin changes.
void IRIntr_Sony(void);

// Called by interrupt routine. Put cmd onto the correct queue
void HandleIR(char cmd);

// Is a command ready on the specified queue?
short IsCmdReady(void);
short IsCmd2Ready(void);
short IsCmd3Ready(void);

// Get the command for the specified queue. Returns 0 if no
// command is available.
unsigned char NextCmd(void);
unsigned char NextCmd2(void);
unsigned char NextCmd3(void);

unsigned char GetLastCmd(unsigned char* cmd);

#endif