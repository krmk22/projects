#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include <util/delay.h>
#include "sim800l.h"

unsigned char GSMArray[60], GSMCount;

ISR(USART_RX_vect)
{
	GSMArray[GSMCount] = Serial_Receive();
	GSMCount = (GSMCount < 60 ?GSMCount+1 :0);
}

char GSM_Initialize()
{
  Serial_Initialize();
  Serial_Enable(True);
  GSM_TalkBack("AT\r\n","OK",2500);
  _delay_ms(500); _delay_ms(500); _delay_ms(500);
  return GSM_TalkBack("ATE0\r\n","OK",2500);
} 

char GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
  GSM_TalkBack("AT+CMGF=1\r\n","OK",2500);
	Serial_Write("AT+CMGS=\"+91"); Serial_Write(MobileNumber);Serial_Write("\"\r\n"); 
	GSM_TalkBack("",">",2500); Serial_Write(SendData); DelayMS(100); Serial_Send(0x1A);
	char ReturnVariable = GSM_TalkBack("","OK",5000);
	GSMCount = 0; memset(GSMArray, '\0', sizeof(GSMArray));
	return ReturnVariable;
}

char GSM_ReceiveMessage(unsigned char *StoreArray, const char Status)
{
	if(smemmem((const void*)GSMArray, GSMCount, "+CMT:", 4) != Null)
	{
		if(smemmem((char*)GSMArray, GSMCount, "+91", 3) != Null)
		{
			DelayMS(100); SplitString((unsigned char*)GSMArray, StoreArray, "+91", '"');
      if(Status == ACK) GSM_SendMessage(StoreArray, "Your Message Received");
			return True;
		}
	}
	return False;
}

char GSM_Incoming(unsigned char *StoreArray, const char Condition)
{
	if(smemmem((const void*)GSMArray, GSMCount, "RING", 4) != Null)
	{
		if(smemmem((char*)GSMArray, GSMCount, "+91", 3) != Null)
		{
			DelayMS(100); SplitString((unsigned char*)GSMArray, StoreArray, "+91", '"');
			if(Condition == ATTEND) return GSM_TalkBack("ATA\r\n","OK", 2500);
			else return GSM_TalkBack("ATH\r\n","OK", 2500);
			memset(GSMArray, '\0', sizeof(GSMArray));
			GSMCount = 0;
			return True;
		}
	}
	return False;
}

char GSM_Outgoing(const char *MobileNumber)
{
  GSM_TalkBack("AT+CLIP=1\r\n","OK",2500);
	Serial_Write("ATD+91");
	Serial_Write(MobileNumber);
	GSM_TalkBack(";\r\n","OK",2500); DelayMS(1000);
	memset(GSMArray, '\0', sizeof(GSMArray)); GSMCount = 0;
  return GSM_TalkBack("AT+CLCC\r\n","OK",2500);
}

char GSM_CheckSignal()
{
  unsigned char MaximumDelay = 50;
  do
  {
    char LocalVariable = GSM_TalkBack("AT+CREG?\r\n","OK",2500);
    if(strstr((char*)GSMArray, "0,0") != Null) return False;
    else if(strstr((char*)GSMArray, "0,3") != Null) return False;
    else if(strstr((char*)GSMArray, "0,4") != Null) return False;
    else if(strstr((char*)GSMArray, "0,1") != Null) return True;
    else if(strstr((char*)GSMArray, "0,5") != Null) return True;
    else if(!LocalVariable) return False;
    _delay_ms(1000);
  }
  while(MaximumDelay --> 0);
  return False;
}

char GSM_OutgoingResult()
{
  GSM_TalkBack("AT+CLCC\r\n","OK",2500);
  if(strstr((char*)GSMArray, "0,0,0,0") != '\0') return ATTD;
  else if(strstr((char*)GSMArray, "0,6,0,0") != '\0') return DECL;
  else if(strstr((char*)GSMArray, "0,2,0,0") != '\0') return DIAL;
  else if(strstr((char*)GSMArray, "0,3,0,0") != '\0') return RING;
  return '\0';
}

char GSM_TalkBack(const char *MainData, const char *FeedData, unsigned int TimeDelay)
{
	memset(GSMArray, '\0', sizeof(GSMArray));
	GSMCount = 0; Serial_Write(MainData);
	do
	{
		if(strstr((char*)GSMArray, FeedData) != Null) return True;
		if(strstr((char*)GSMArray, "ERROR") != Null) return False;
		_delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}	
	
