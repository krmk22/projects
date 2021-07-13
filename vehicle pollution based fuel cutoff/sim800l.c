#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif

#ifndef _UART_H
#include "uart.h"
#endif

#include <util/delay.h>
#include "sim800l.h"

unsigned char GSMArray[60], GSMCount;
const char *GSM2Ptr = (char*)GSMArray;

ISR(USART_RX_vect)
{
	GSMArray[GSMCount] = Serial_Receive();
	GSMCount = (GSMCount < 60 ?GSMCount+1 :0);
}

void GSM_Initialize()
{
	Serial_Initialize();
	Serial_Enable(1);
	sei(); 
	
	GSMCount = 0; LCDClear(); 
	memset(GSMArray, '\0', sizeof(GSMArray));
	if(GSM_TalkBack("AT\r\n","OK",2500)) LCD_Disp(0x80,"GSM FOUND");
	else LCD_Disp(0x80,"GSM NOT FOUND"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("ATE0\r\n","OK",2500)) LCD_Disp(0x80,"ECHO CLEAR");
	else LCD_Disp(0x80,"ECHO NOT CLEAR"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CSMINS?\r\n","0,1",2500)) LCD_Disp(0x80,"SIM FOUND");
	else LCD_Disp(0x80,"SIM NOT FOUND"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CPIN?\r\n","READY",2500)) LCD_Disp(0x80,"SECURE ACCESS");
	else LCD_Disp(0x80,"SIM PIN LOCK"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CREG?\r\n","0,2",2500)) LCD_Disp(0x80,"N/W SEARCHING");
	else LCD_Disp(0x80,"N/W SEARCH ERROR"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CREG?\r\n","0,1",2500)) LCD_Disp(0x80,"SIM GOT TOWER");
	else LCD_Disp(0x80,"N/W FAILED"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CMGF=1\r\n","OK",2500)) LCD_Disp(0x80,"MESSAGE READY");
	else LCD_Disp(0x80,"MESSAGE ERROR"); DelayMS(1000); LCDClear();
	
	/*
	if(GSM_TalkBack("AT+CNMI=2,2,0,0,1\r\n","OK",2500)) LCD_Disp(0x80,"TERMINAL READY");
	else LCD_Disp(0x80,"TERMINAL ERROR"); DelayMS(1000); LCDClear();
	*/

	if(GSM_TalkBack("AT+CLIP=1\r\n","OK",2500)) LCD_Disp(0x80,"CALLER ID READY");
	else LCD_Disp(0x80,"CALLER ID ERROR"); DelayMS(1000); LCDClear();
}	

char GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
	Serial_Write("AT+CMGS=\"+91"); Serial_Write(MobileNumber);Serial_Write("\"\r\n"); 
	GSM_TalkBack("",">",2500); Serial_Write(SendData); DelayMS(100); Serial_Send(0x1A);
	char ReturnVariable = GSM_TalkBack("","OK",5000);
	GSMCount = 0; memset(GSMArray, '\0', sizeof(GSMArray));
	return ReturnVariable;
}

char GSM_ReceiveMessage(unsigned char *StoreArray)
{
	if(smemmem((const void*)GSM2Ptr, GSMCount, "+CMT:", 4) != Null)
	{
		if(smemmem(GSM2Ptr, GSMCount, "+91", 3) != Null)
		{
			DelayMS(100); SplitString((unsigned char*)GSMArray, StoreArray, "+91", '"');
			return True;
		}
	}
	return False;
}

char GSM_Incoming(unsigned char *StoreArray, const char Condition)
{
	if(smemmem((const void*)GSM2Ptr, GSMCount, "RING", 4) != Null)
	{
		if(smemmem(GSM2Ptr, GSMCount, "+91", 3) != Null)
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
	unsigned char MaximumDelay = 20;
	
	Serial_Write("ATD+91");
	Serial_Write(MobileNumber);
	GSM_TalkBack(";\r\n","OK",2500); DelayMS(1000);
	memset(GSMArray, '\0', sizeof(GSMArray)); GSMCount = 0;
	LCD_Disp(0xC0,"                ");
	LCD_Disp(0xC0,(char*)MobileNumber);
	
	do
	{
		GSM_TalkBack("AT+CLCC\r\n","OK",2500);
		if(smemmem(GSM2Ptr, GSMCount, "0,0,0,0", 6) != Null)
		{
			LCD_Disp(0xCB,"ATTND");
			return True;
		}
		if(smemmem(GSM2Ptr, GSMCount, "0,6,0,0", 6) != Null)
		{
			LCD_Disp(0xCB,"DECLI");
			return False;
		}
		if(smemmem(GSM2Ptr, GSMCount, "0,2,0,0", 6) != Null)
		LCD_Disp(0xCB, "DIAL ");
		if(smemmem(GSM2Ptr, GSMCount, "0,3,0,0", 6) != Null)
		LCD_Disp(0xCB, "RING ");
		DelayMS(1000);
	}
	while(MaximumDelay --> 0);
	return False;
}

char GSM_TalkBack(const char *MainData, const char *FeedData, unsigned int TimeDelay)
{
	memset(GSMArray, '\0', sizeof(GSMArray));
	GSMCount = 0; Serial_Write(MainData);
	
	do
	{
		if(smemmem(GSM2Ptr, GSMCount, FeedData, strlen(FeedData)) != Null) return True;
		if(smemmem(GSM2Ptr, GSMCount, "ERROR", 5) != Null) return False;
		_delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}	
	
