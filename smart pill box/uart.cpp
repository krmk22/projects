#include <Arduino.h>
#include <avr/io.h>
#include <string.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#include "uart.h"

unsigned char SerialArray[250], SerialCount;

ISR(USART_RX_vect)
{
  SerialArray[SerialCount] = Serial_Receive();
  SerialCount = (SerialCount < 250 ?SerialCount+1 :0);
}

void Serial_Initialize(unsigned long ReqBaud)
{
  unsigned char RegCalculation = (((F_CPU / (ReqBaud * 16UL))) - 1);
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
	Serial_Send(Data[k]);
}

void Serial_Decimal(unsigned int DecimalValue, unsigned int Length, const char Base)
{
	register unsigned char LocalArray[10];
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

void Serial_Flush()
{
  memset(SerialArray, '\0', sizeof(SerialArray));
  SerialCount = 0;
}

char STalkBack(const char LineState, const char *TBString, unsigned char Length, unsigned int TimeDelay)
{
  if(LineState) Serial_Write("\r\n");
  
  do
  {
    if(smemmem((char*)SerialArray, SerialCount, TBString, Length) != '\0') return True;
    if(smemmem((char*)SerialArray, SerialCount, "ERROR", 5) != '\0') return False;
    delay(1);
  } 
  while(TimeDelay --> 0);
  return False;
}

