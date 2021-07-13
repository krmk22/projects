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

#include "uart.h"

void Serial_Initialize()
{
	SPBRG = (unsigned char)RegCalculation;
	TXEN = 1;
	SYNC = 0;
	BRGH = 1;
	TRMT = 1;
	SPEN = 1;
}

void Serial_Send(unsigned char Data)
{
	while(!TXIF);
	TXIF = 0;
	TXREG = Data;
}

void Serial_Write(const char *Data)
{
	while(*Data) Serial_Send(*Data++);
}

void Serial_Writeln(const char *SendData, unsigned int Length)
{
	for(unsigned int k = 0; k < Length; k++)
	Serial_Send(SendData[k]);
}

void Serial_Decimal(unsigned long int DecimalValue, unsigned char Length, const Base)
{
	unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	Serial_Write((char*)LocalArray);
}

void Serial_Float(float Value, unsigned char PostFix, unsigned char PreFix, const Seg)
{
	unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	Serial_Write((char*)LocalArray);
}

void Serial_Enable(const char Condition)
{
	if(Condition == True) RCIE = CREN = 1;
	if(Condition == False) RCIE = CREN = 0;
}					
