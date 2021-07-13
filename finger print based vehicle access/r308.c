#include <pic.h>
#include <stdlib.h>
#include <string.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _INT_H
#include "int.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include "r308.h"

void FP_Routine()
{
	FPArray[FPCount] = RCREG;
	FPCount = (FPCount < 20 ?FPCount + 1 :0);
	RCIF = 0;
}	

void FP_Initialize()
{
	Serial_Initialize();
	Serial_Enable(True);
	EnableInterrupt(RXRINT, FP_Routine);
	GIE = PEIE = True;
}

char CollectFingerImage()
{
  memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x01\x00\x05", 12);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 5000));
}

char ConvertImage2Char(unsigned char PageID)
{
	unsigned int CheckSum = PageID + 7;
	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
	Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x02", 10);
    Serial_Send(PageID); Serial_Send(0x00); Serial_Send(CheckSum);
    return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\07\x00\x03\x00", 10, 2500));
} 

char FingerEnrollMent(unsigned char FingerID)
{
	unsigned char CheckSum = 15 + FingerID;
	CollectFingerImage(); DelayMS(500);
	ConvertImage2Char(1); DelayMS(500);
	CollectFingerImage(); DelayMS(500);
	ConvertImage2Char(2); DelayMS(500);
	
	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
 	Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\05\x00\x09", 12);
  	if(!FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500)) return False;
  	DelayMS(500);
  	
  	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
  	Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x06\x02", 11);
  	Serial_Send(0x00); Serial_Send(FingerID); Serial_Send(0x00); Serial_Send(CheckSum);
  	if(!FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500)) return False;
  	DelayMS(500);
  	
  	return True;
}

char FingerIdentify(unsigned char *FingerID, unsigned char *MatchingScore)
{
	CollectFingerImage(); DelayMS(500);
	ConvertImage2Char(1); DelayMS(500);
	
	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
    Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x08\x1B", 10);
    Serial_Writeln("\x01\x00\x00\x01\x01\x00\x27", 7);
    if(!FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x07\x00", 10, 2500)) return False;
    DelayMS(500);
    
    *MatchingScore = FPArray[13];
    *FingerID = FPArray[11];
    return True;
}

char FingerVerify(unsigned char FingerID, unsigned char *MatchingScore) 
{
	unsigned char CheckSum = FingerID + 16;
	
	CollectFingerImage(); DelayMS(500);
	ConvertImage2Char(1); DelayMS(500);
	
	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
  	Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x07\x02", 11);
  	Serial_Send(0x00); Serial_Send(FingerID); Serial_Send(0x00); Serial_Send(CheckSum);
  	if(!FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500)) return False;
  	DelayMS(500);
  	
  	memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
    Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x03\x00\x07", 12);
    if(!FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x05\x00", 10, 2500)) return False;
    DelayMS(500);
    
    *MatchingScore = FPArray[11];
    return True;
}

char FingerDelete(unsigned char FingerID)
{
  unsigned char CheckSum = FingerID + 21;
  memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x07\x0C", 10);
  Serial_Send(0x00); Serial_Send(FingerID); Serial_Writeln("\x00\x01\x00",3); Serial_Send(CheckSum);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500));
}

char FingerDeleteAll()
{
  memset(FPArray, '\0', sizeof(FPArray)); FPCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x0D\x00\x11", 12);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500));
}

char FingerTalkBack(const char *FeedData, unsigned int FDLength, unsigned int TimeDelay)
{
	do
	{
		if(smemmem((char*)FPArray, FPCount, FeedData, FDLength) != '\0') return True;
		if(FPCount > 10 && FPArray[9] != 0x00) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}