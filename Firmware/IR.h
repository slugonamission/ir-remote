#ifndef __IR_H__
#define __IR_H__

#include "GenericTypeDefs.h"

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
short IsMouseReady(void);

// Get the command for the specified queue. Returns 0 if no
// command is available.
unsigned char NextCmd(void);
unsigned char NextCmd2(void);
unsigned char NextCmd3(void);
unsigned char NextMouse(void);

unsigned char GetLastCmd(unsigned char* cmd);

/*void ReadEEPROMTable(void);
void WriteEEPROMTable(void);
void AddTableEntry(UINT8 report, UINT8 ir_cmd, UINT8 usb_cmd);
void RemoveTableEntry(UINT8 ir_cmd);
*/

#endif