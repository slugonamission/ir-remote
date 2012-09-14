#include "Timer.h"
#include "Compiler.h"
#include "HardwareProfile.h"

void TMR0StartReset(void)
{
	TMR0H = 0;
	TMR0L = 0;
	T0CON |= 0x80; // Enable it
}

void TMR0Stop(void)
{
	T0CON &= 0x7F; // Disable it
}

int TMR0Val(void)
{
	return TMR0L;
}

void TMR2StartReset(void)
{
	TMR2 = 0;
	T2CON |= 0x04; // Start the timer
}

void TMR2Stop(void)
{
	TMR2 = 0; // Just incase
	T2CON &= 0xFB; // Disable timer
}

int TMR2Val(void)
{
	return TMR2;
}