#include <at89x52.h>
#include <intrins.h>

#ifndef _HEADER_H
#include "header.h"
#endif

#include "servo.h"

void Timer2_ISR() interrupt 5
{
	unsigned int FinalPosition;
	TR2 = 0;
	FinalPosition = (unsigned int)MapValue(ServoPosition, 
	MinimumAngle, MaximumAngle, MinimumPulse, MaximumPulse);
	TMOD |= (1 << 4); TH1 = TL1 = 0; TR1 = 1;
	ServoPort |= (1 << ServoPin);
	while(((TH1 << 8) | TL1) <= FinalPosition);
	ServoPort &=~ (1 << ServoPin); TR1 = 0;
	TH2 = 0xB1; TL2 = 0xDC; TF2 = 0; TR2=1;
	return ;
}

void Servo_Initialize()
{
	ServoPort &=~ (1 << ServoPin);
	EA = 1;
	T2CON = 0x00;
	T2MOD = 0x00;
	TH2 = 0xB1; 
	TL2 = 0xDF; 
	ET2 = 1;
	TR2 = 1;
}