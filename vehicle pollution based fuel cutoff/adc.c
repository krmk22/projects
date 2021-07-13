#include <avr/io.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#include <util/delay.h>
#include "adc.h"

void ADC_Initialize()
{
	ADMUX |= (1 << REFS0); //AVCC as Aref
	ADCSRA |= (1 << ADPS2)|(1 << ADPS1)|(1 << ADPS0); //128 Prescaler
	ADCSRA |= (1 << ADEN); //Enable Analog Conversion
}

unsigned int ReadADC(char Channel)
{
	ADMUX = (ADMUX & 0xF0) | (Channel & 0x0F); //Channel with Mask
	ADCSRA |= (1 << ADSC); //Select Single Channel Conversion
	while(ADCSRA & (1 << ADSC)); //Wait For ADC Conversopn
	return ADC;
}

unsigned int ReadRawADC(char Channel, char Samples)
{
	unsigned long int ReturnVariable = 0;
	for(unsigned char LoopVariable = 0; LoopVariable < Samples; LoopVariable++)
	ReturnVariable = ReturnVariable + ReadADC(Channel);
	return ReturnVariable / Samples;
}


