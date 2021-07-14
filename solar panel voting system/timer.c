#include <htc.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#include "timer.h"

volatile unsigned long int MicroTicks;
volatile unsigned long int MilliTicks;

void TimerRoutine()
{
	MilliTicks += 50;
	MicroTicks += 50000;
	WRITETIMER3(TIMER3REG);
	TMR3IF = 0; 
}	

void Timer_Initialize()
{
	T3RD16 = True;
	T3CCP2 = T3CCP1 = False;
	T3CKPS1 = T3CKPS0 = True;
	T3SYNC = TMR3CS = False;

	WRITETIMER3(TIMER3REG);
    EnableInterrupt(TIMER3, TimerRoutine);
    ActivateInterrupt();
    TMR3IE = TMR3ON = True;
}

unsigned long Millis()
{
	DeactivateInterrupt();
	register unsigned long ReturnVariable = MilliTicks + ((READTIMER3() - TIMER3REG) / 1000);
	ActivateInterrupt();
	return ReturnVariable;
}	

unsigned long Micros()
{
	DeactivateInterrupt();
	register unsigned long ReturnVariable = MicroTicks + (READTIMER3() - TIMER3REG);
	ActivateInterrupt();
	return ReturnVariable;
}			