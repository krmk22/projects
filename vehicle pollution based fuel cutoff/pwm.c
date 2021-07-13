#include <avr/io.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#include <util/delay.h>
#include "pwm.h"

void PulseWidthModulation1A(unsigned char RequirePulse)
{
	PulseWidth1ADirection |= (1 << PulseWidth1A);
	TCCR0A |= (1 << COM0A1); //Non Inverting Mode
	TCCR0A |= (1 << WGM01) | (1 << WGM00); //Fast PWM 8Bit 
	TCCR0B |= (1 << CS01); //8Bit Prescalar
	OCR0A = RequirePulse;
}

void PulseWidthModulation1B(unsigned char RequirePulse)
{
	PulseWidth1BDirection |= (1 << PulseWidth1B);
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << WGM01) | (1 << WGM00);
	TCCR0B |= (1 << CS01);
	OCR0B = RequirePulse;
}

void PulseWidthModulation2A(unsigned char RequirePulse)
{
	PulseWidth2ADirection |= (1 << PulseWidth2A);
	TCCR1A |= (1 << COM1A1);
	TCCR1A |= (1 << WGM10);
	TCCR1B |= (1 << CS11);
	OCR1A = RequirePulse;
}

void PulseWidthModulation2B(unsigned char RequirePulse)
{
	PulseWidth2BDirection |= (1 << PulseWidth2B);
	TCCR1A |= (1 << COM1B1);
	TCCR1A |= (1 << WGM10);
	TCCR1B |= (1 << CS11);
	OCR1B = RequirePulse;
}

void PulseWidthModulation3A(unsigned char RequirePulse)
{
	PulseWidth3ADirection |= (1 << PulseWidth3A);
	TCCR2A |= (1 << COM2A1);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= (1 << CS21);
	OCR2A = RequirePulse;
}

void PulseWidthModulation3B(unsigned char RequirePulse)
{
	PulseWidth3BDirection |= (1 << PulseWidth3B);
	TCCR2A |= (1 << COM2B1);
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	TCCR2B |= (1 << CS21);
	OCR2B = RequirePulse;
}
