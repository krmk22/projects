#ifndef _TIMER_H
#define _TIMER_H

#define MS				/1000
#define REQTIMER 		50 MS
#define PRESCALE		8
#define TIMER3REG		(65536 - ((_XTAL_FREQ * REQTIMER) / (PRESCALE * 4)))

extern volatile unsigned long int MilliTicks;
extern volatile unsigned long int MicroTicks;

extern void Timer_Initialize();
extern unsigned long Millis();
extern unsigned long Micros();
#endif