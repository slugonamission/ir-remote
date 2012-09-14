#ifndef __TIMER_H__
#define __TIMER_H__

void TMR0StartReset(void);
void TMR0Stop(void);
int TMR0Val(void);

void TMR2StartReset(void);
void TMR2Stop(void);
int TMR2Val(void);

#endif