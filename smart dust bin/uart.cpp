#include <avr/io.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#include "uart.h"

void Serial_Initialize()
{
	UBRR0H = (unsigned char) RegCalculation >> 8;
	UBRR0L = (unsigned char) RegCalculation & 0xFF;
	UCSR0A &=~ (1 << U2X0);
	UCSR0B |= (1 << TXEN0);
	UCSR0C |= (0 << USBS0)|(3 << UCSZ00);
}

void Serial_Send(unsigned char SendData)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = SendData;
}

void Serial_Write(const char *SendData)
{
	while(*SendData) Serial_Send(*SendData++);
}

void Serial_Writeln(const char *Data, unsigned int Length)
{
	for(int k=0; k<Length; k++)
	{
		Serial_Send(Data[k]);
	}
}

void Serial_Decimal(unsigned long int DecimalValue, unsigned char Length, const char Base)
{
	unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	Serial_Write((char*)LocalArray);
}

void Serial_Float(float Value, char PostFix, char PreFix, const char Seg)
{
	unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	Serial_Write((char*)LocalArray);
}

void Serial_Enable(char Condition)
{
	if(Condition) UCSR0B |= (1 << RXCIE0)|(1 << RXEN0);
	else UCSR0B &=~ ((1 << RXCIE0)|(1 << RXEN0));
}

unsigned char Serial_Receive()
{
	while(!(UCSR0A & (1 << UDRE0)));
	return UDR0;
}

