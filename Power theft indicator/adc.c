#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#include "adc.h"

unsigned char ReadADC8(unsigned char Channel)
{
	ADCON1 = ((ClockSelect & (1 << 2)) << 6);
	ADCON0 = (((ClockSelect & (1 << 1)) << 7) | ((ClockSelect & (1 << 0)) << 6));
	ADCON0 |= (Channel << 3);
	ADCON0 |= (1 << 0); 
	__delay_us(Oscillation * MinimumTadTime);
	ADCON0 |= (1 << 2);
	__delay_us(MinimumAcquistionTime);
	while(ADCON0 & (1 << 2));
	return ADRESH;
}

unsigned int ReadADC10(unsigned char Channel)
{
	ADCON1 = ((1 << 7) | ((ClockSelect & (1 < 2)) << 6));
	ADCON0 = (((ClockSelect & (1 << 1)) << 7) | ((ClockSelect & (1 << 0)) << 6)); 
	ADCON0 |= (Channel << 3);
	ADCON0 |= (1 << 0);
	__delay_us(Oscillation * MinimumTadTime);
	ADCON0 |= (1 << 2);
	__delay_us(MinimumAcquistionTime);
	while(ADCON0 & (1 << 2));
	return ((ADRESH << 8) | ADRESL);
}

unsigned char ReadRawADC8(unsigned char Channel, unsigned char Count)
{
	unsigned long int ReturnVariable = 0;
	for(unsigned char LoopVariable = 0; LoopVariable < Count; LoopVariable++)
	ReturnVariable = ReturnVariable + ReadADC8(Channel);
	return ReturnVariable / Count;
}

unsigned int ReadRawADC10(unsigned char Channel, unsigned char Count)
{
	unsigned long int ReturnVariable = 0;
	for(unsigned char LoopVariable = 0; LoopVariable < Count; LoopVariable++)
	ReturnVariable = ReturnVariable + ReadADC10(Channel);
	return ReturnVariable / Count;
}		