#ifndef _CLOCK_H
#define _CLOCK_H

/*
We Don't Need to Declare Each and
Every Header About The Clocking 
Cycle. This File is Enough to Carry
OverAll Details About The
Clocking Cycle of The Processor
*/

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 20000000UL
#endif

#ifndef OneMachineCycle
#define OneMachineCycle ((float)4000000UL / (float)_XTAL_FREQ)
#endif

#define Serial 1
#define SSerial 0

#define FIVE_KEYPAD 1
#define MATRIX_KEYPAD 0 

#ifndef DelayMS
#define DelayMS(Delay)\
{\
	unsigned char RoutineDelay;\
	unsigned int RequireDelay = Delay;\
	do\
	{\
		RoutineDelay = 4;\
		do\
		{\
			__delay_us(250);\
		}\
		while(--RoutineDelay);\
	}\
	while(--RequireDelay);\
}
#endif
#endif		