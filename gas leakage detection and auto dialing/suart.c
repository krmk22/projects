#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#include "suart.h"

void SSerial_Initialize()
{
	SerialDirection |= (1 << Receiver);
	SerialDirection &=~ (1 << Transmitter);
	SerialPort |= (1 << Transmitter);
	
	GIE = PEIE = 1;
	CCP1CON = (1 << 2);
	CCP1IF = 0;
}	

void SSerial_Send(unsigned char Data)
{
	GIE = 0; //<-- Require ShutDown All Interrutps
	SerialPort &=~ (1 << Transmitter);
	__delay_us(FCallDelay - BasicDelay);
	
	for(unsigned char Shifting = 0x01; Shifting; Shifting <<= 1)
	{
		if(Data & Shifting) SerialPort |= (1 << Transmitter);
		else SerialPort &=~ (1 << Transmitter);
		__delay_us(FLoopDelay);
	}
	
	SerialPort |= (1 << Transmitter);
	GIE = 1;
	__delay_us(FStopDelay - BasicDelay);
}

void SSerial_Write(const char *Data)
{
	while(*Data) SSerial_Send(*Data++);
}

void SSerial_Writeln(const char *Data, unsigned int Length)
{
	for(unsigned int k = 0; k < Length; k++)
	SSerial_Send(Data[k]);
}

unsigned char SSerial_Receive()
{
	unsigned char ReturnVariable = 0;
	for(unsigned char Shifting = 0x01; Shifting; Shifting <<= 1)
	{
		__delay_us(FIntDelay);
		if(SSerialReceive) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting;
	}
	__delay_us(FStopDelay);
	while(!SSerialReceive);
	return ReturnVariable;
}

void SSerial_Decimal(unsigned long int DecimalValue, unsigned char Length, const Base)
{
	unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	SSerial_Write((char*)LocalArray);
}

void SSerial_Float(float Value, unsigned char PostFix, unsigned char PreFix, const Seg)
{
	unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	SSerial_Write((char*)LocalArray);
}

void SSerial_Enable(const Condition)
{
	if(Condition == True) CCP1IE = 1;
	if(Condition == False) CCP1IE = 0;
}	
