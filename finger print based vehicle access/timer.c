#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#include "timer.h"

void TimerRoutine()
{
	MilliTicks++;
	TMR0 = (((unsigned char)TiMeR0) + 2);
	TMR0IF = 0;
}

void StartTimer()
{
	GIE = PEIE = 1;
	MilliTicks = 0;
	OPTION_REG = (1 << 2);
	TMR0 = (unsigned char)TiMeR0;
	EnableInterrupt(TIMER0, TimerRoutine);
	TMR0IE = 1;
}

void StopTimer()
{
	TMR0IE = 0;
	DisableInterrupt(TIMER0);
	LastTicks = MilliTicks;
}	

void RestartTimer()
{
	MilliTicks = LastTicks;
	TMR0 = (unsigned char)TiMeR0;
	EnableInterrupt(TIMER0, TimerRoutine);
	TMR0IE = 1;
}	

void ResetTimer()
{
	TMR0IE = 0;
	MilliTicks = 0;
	LastTicks = 0;
	TMR0 = 0;
	DisableInterrupt(TIMER0);
	TMR0 = (unsigned char)TiMeR0;
	TMR0IE = 1;
	EnableInterrupt(TIMER0, TimerRoutine);
}	
		