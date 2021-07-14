#include <htc.h>
#include <string.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#include "suart.h"

unsigned char SSerialArray[100], SSerialCount;

static unsigned int FCallDelay;
static unsigned int FIntDelay;
static unsigned int BasicDelay;
static unsigned int FStopDelay;
static unsigned int FLoopDelay;
static unsigned int BitPerUS;

void SSerialRoutine()
{
	SSerialArray[SSerialCount] = SSerial_Receive();
	SSerialCount = (SSerialCount < 100 ?SSerialCount + 1 :0);
	CCP2IF = 0;
}

void SSerial_Initialize(unsigned long ReqBaud)
{
	BitPerUS = 1000000UL / ReqBaud;
	FCallDelay = (BitPerUS - (unsigned)(10 * OneMachineCycle));
	FIntDelay =  (BitPerUS - (unsigned)(10 * OneMachineCycle)); 
	BasicDelay = (unsigned)(2 * OneMachineCycle);
	FLoopDelay = FStopDelay = BitPerUS;
	
	SerialDire |= (1 << Receiver);
	SerialDire &=~(1 << Transmitter);
	SerialPort |= (1 << Transmitter);
	
	CCP2CON = (1 << 2); CCP2IF = 0;
	EnableInterrupt(CP2INT, SSerialRoutine);
	ActivateInterrupt();
}

void SSerial_Send(unsigned char Data)
{
	DeactivateInterrupt(); TXR = 0;
	DelayUS((unsigned)(FCallDelay - BasicDelay));
	
	for(register unsigned char Shifting = 0x01; Shifting; Shifting <<= 1)
	{
		if(Data & Shifting) TXR = 1;
		else TXR = 0; 
		DelayUS((unsigned)FLoopDelay);
	}
	
	TXR = 1; ActivateInterrupt(); 
	DelayUS((unsigned)(FStopDelay - BasicDelay));
}

void SSerial_Write(const char *Data)
{
	while(*Data)
	SSerial_Send(*Data++);
}

void SSerial_Writeln(const char *Data, unsigned int Length)
{
	for(register unsigned int k = 0; k < Length; k++)
	SSerial_Send(Data[k]);
}

unsigned char SSerial_Receive()
{
	volatile unsigned char ReturnVariable = 0x00;
	if(RXR) return ReturnVariable;
	
	for(register unsigned char Shifting = 0x01; Shifting; Shifting <<= 1)
	{	
		DelayUS((unsigned)FIntDelay); 
		if(RXR) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting;
	}
	DelayUS((unsigned)FStopDelay);
	//while(!RXR);
	return ReturnVariable;
}	

void SSerial_Decimal(unsigned long int DecimalValue, int Length, int Base)
{
	register unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	SSerial_Write((char*)LocalArray);
}

void SSerial_Float(float Value, int PostFix, int PreFix, int Seg)
{
	register unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	SSerial_Write((char*)LocalArray);
}	

void SSerial_Enable(int Condition)
{
	CCP2IE = Condition;
}

void SSerial_Flush()
{
	memset(SSerialArray, '\0', sizeof(SSerialArray));
	SSerialCount = 0;
}	

char SSTalkBack(int LineState, const char *TBString, unsigned char Length, unsigned int TimeDelay)
{
	if(LineState) SSerial_Write("\r\n");
	
	do
	{
		if(smemmem((char*)SSerialArray, SSerialCount, TBString, Length) != '\0') return True;
		if(smemmem((char*)SSerialArray, SSerialCount, "ERROR", 5) != '\0') return False;
		DelayMS(1);
	}
	while(TimeDelay --> 0);
	return False;	
}	