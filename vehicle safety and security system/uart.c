#include <htc.h>
#include <string.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#include "uart.h"

unsigned char SerialArray[100], SerialCount;

void SerialRoutine()
{
	SerialArray[SerialCount] = RCREG;
	SerialCount = (SerialCount < 100 ?SerialCount + 1 :0);
	RCIF = 0;
}

void Serial_Initialize(unsigned long ReqBaud)
{	
	register unsigned long FinalBaud = (((_XTAL_FREQ / (ReqBaud * 4))) - 1);
	
	SPBRGH = (unsigned int)((FinalBaud >> 8) & 0xFF);
	SPBRG  = (unsigned int) (FinalBaud & 0xFF);
	
	TX9 = 0; SYNC = 0;
	TX9D = 0; BRGH = 1;
	SPEN = 1; RX9 = 0;
	ADDEN = 0; BRG16 = 1;
	ABDEN = 0; RCIDL = 0; 
	WUE = 0; TXEN = 1;
	
	EnableInterrupt(RXRINT, SerialRoutine);
	ActivateInterrupt();
}

void Serial_Send(unsigned char Data)
{
	while(!TRMT) continue;
	TXREG = Data;
}

void Serial_Write(const char *Data)
{
	while(*Data)
	Serial_Send(*Data++);
}

void Serial_Writeln(const char *SendData, unsigned int Length)
{
	for(register unsigned int k = 0; k < Length; k++)
	Serial_Send(SendData[k]);
}

void Serial_Decimal(unsigned long DecimalValue, int Length, int Base)
{
	register unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	Serial_Write((char*)LocalArray);
}

void Serial_Float(float Value, int PostFix, int PreFix, int Seg)
{
	register unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	Serial_Write((char*)LocalArray);
}

void Serial_Enable(int Condition)
{
	RCIE = Condition;
	CREN = Condition;
}

unsigned char Serial_Receive()
{
	while(!RCIF) continue;
	return RCREG;
}

void Serial_Flush()
{
	memset(SerialArray, '\0', sizeof(SerialArray));
	SerialCount = 0;
}	

char STalkBack(int LineState, const char *TBString, unsigned char Length, unsigned int TimeDelay)
{
	if(LineState) Serial_Write("\r\n");
	
	do
	{
		if(smemmem((char*)SerialArray, SerialCount, TBString, Length) != '\0') return True;
		if(smemmem((char*)SerialArray, SerialCount, "ERROR", 5) != '\0') return False;
		DelayMS(1);
	}	
	while(TimeDelay --> 0);
	return False;
}	