#ifndef _CLOCK_H
#define _CLOCK_H

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>

#ifndef OneMachineCycle
#define OneMachineCycle ((float)1000000UL / (float)F_CPU)
#endif

#ifndef Number4Communication
#define Number4Communication
#define DSerial 1
#define SSerial 2
#endif

#ifndef Keyboard4Communication
#define Keyboard4Communication
#define MATRIX_KEYPAD 1
#define FIVE_KEYPAD 2
#endif

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
			_delay_us(250);\
		}\
		while(--RoutineDelay);\
	}\
	while(--RequireDelay);\
}
#endif
#endif
