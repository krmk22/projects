#include <pic.h>
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

#ifndef _LCD_H
#include "lcd.h"
#endif

#include "sim800l.h"

#if MC2GSM == Serial
#ifndef _UART_H
#include "uart.h"
#endif
#else
#ifndef _SUART_H
#include "suart.h"
#endif
#endif

#if MC2GSM == Serial
void GSMRoutine()
{
	GSMArray[GSMCount] = RCREG;
	GSMCount = (GSMCount < 60 ?GSMCount+1 :0);
	RCIF = 0;
}
#else
void GSMRoutine()
{
	if(!SSerialReceive)
	{
		GSMArray[GSMCount] = SSerial_Receive();
		GSMCount = (GSMCount < 60 ?GSMCount+1 :0);
	}
	CCP1IF = 0;
}
#endif

char GSM_Initialize()
{	
	#if MC2GSM == Serial
	Serial_Initialize();
	Serial_Enable(True);
	EnableInterrupt(RXRINT, GSMRoutine);
	#else
	SSerial_Initialize();
	SSerial_Enable(True);
	EnableInterrupt(CP1INT, GSMRoutine);
	#endif
	
	GIE = 1; PEIE = 1; LCDClear(); GSMCount = 0;
	memset(GSMArray, '\0', sizeof(GSMArray));
	return False;
	/*
	if(GSM_TalkBack("AT\r\n","OK",2500) == False) return NO_GSM;
	if(GSM_TalkBack("ATE0\r\n","OK",2500) == False) return NO_GSM;
	if(GSM_TalkBack("AT+CSMINS?\r\n","0,1",2500) == False) return NO_SIM;
	if(GSM_TalkBack("AT+CPIN?\r\n","READY",2500) == False) return NO_SIM;
	if(GSM_CheckSignal() == False) return NO_NET;
	if(GSM_TalkBack("AT+CMGF=1\r\n","OK",2500) == False) return NO_NET;
	GSM_TalkBack("AT+CNMI=2,2,0,0,0\r\n","OK",2500);
	GSM_TalkBack("AT+CLIP=1\r\n","OK",2500);
	return GSM_OK;*/
}

char GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
	/*
	LCD_Disp(0xC0,"                ");
	LCD_Disp(0xC0, MobileNumber);
	LCD_Disp(0xCB, "SEND");
	//GSM_TalkBack("AT+CMGF=1\r\n", "OK", 2500);
	#if MC2GSM == Serial
	Serial_Write("AT+CMGS=\"+91");
	Serial_Write(MobileNumber);
	GSM_TalkBack("\"\r\n",">",2500);
	Serial_Write(SendData);
	DelayMS(100);
	Serial_Send(0x1A);
	#else
	SSerial_Write("AT+CMGS=\"+91");
	SSerial_Write(MobileNumber);
	GSM_TalkBack("\"\r\n",">",2500);
	SSerial_Write(SendData);
	DelayMS(100);
	SSerial_Send(0x1A);
	#endif
	*/
	
	SSerial_Write("AT+CMGS=\"+91"); SSerial_Write(MobileNumber);
	SSerial_Write("\"\r\n"); GSM_TalkBack("",">",2500);
	SSerial_Write(SendData); DelayMS(100); SSerial_Send(0x1A);
	char ReturnVariable = GSM_TalkBack("","OK",5000);
	GSMCount = 0; memset(GSMArray, '\0', sizeof(GSMArray));
	LCD_Disp(0xC0,"                ");
	return ReturnVariable;
}

char GSM_ReceiveMessage(unsigned char *StoreArray, const char Status)
{
	if(smemmem((char*)GSMArray, GSMCount, "+CMT:", 4) != Null)
	{
		if(strstr((char*)GSMArray, "+91") != Null)
		{
			DelayMS(100); SplitString((char*)GSMArray, StoreArray, "+91", '"');
			if(Status == ACK) GSM_SendMessage(StoreArray, "Your Message Received");
			return True;
		}
	}
	return False;
}

char GSM_Incoming(unsigned char *StoreArray, const char Status)
{
	if(smemmem((char*)GSMArray, GSMCount, "RING", 4) != Null)
	{
		if(strstr((char*)GSMArray, "+91") != Null)
		{
			DelayMS(100); SplitString((char*)GSMArray, StoreArray, "+91", '"');
			if(Status == ATTEND) return GSM_TalkBack("ATA\r\n","OK",2500);
			else return GSM_TalkBack("ATH\r\n","OK",2500);
		}
	}
	return False;
}

char GSM_Outgoing(const char *MobileNumber)
{
	unsigned char MaximumDelay = 20;
	
	#if MC2GSM == Serial
	Serial_Write("ATD+91");
	Serial_Write(MobileNumber);
	#else
	SSerial_Write("ATD+91");
	SSerial_Write(MobileNumber);
	#endif
	
	GSM_TalkBack(";\r\n","OK",2500); DelayMS(1000);
	memset(GSMArray, '\0', sizeof(GSMArray)); GSMCount = 0;
	LCD_Disp(0xC0,"                ");
	LCD_Disp(0xC0,(char*)MobileNumber);	
	
	do
	{
		GSM_TalkBack("AT+CLCC\r\n","OK",2500);
		
		if(strstr((char*)GSMArray, "0,0,0,0") != Null)
		{
			LCD_Disp(0xCB, "ATTND");
			return True;
		}
		if(strstr((char*)GSMArray, "0,6,0,0") != Null)
		{
			LCD_Disp(0xCB, "DECLI");
			return False;
		}
		if(strstr((char*)GSMArray, "0,2,0,0") != Null)
		LCD_Disp(0xCB, "DIALI");
		if(strstr((char*)GSMArray, "0,3,0,0") != Null)
		LCD_Disp(0xCB, "RINGI");
		DelayMS(1000);
	}
	while(MaximumDelay --> 0);
	return False;
}

char GSM_TalkBack(const char *MainData, const char *FeedData, unsigned int TimeDelay)
{
	memset(GSMArray, '\0', sizeof(GSMArray)); GSMCount = 0;
	
	/*
	#if MC2GSM == Serial
	Serial_Write(MainData);
	#else
	SSerial_Write(MainData);
	#endif
	*/
	
	SSerial_Write(MainData);			
	
	do
	{
		if(strstr((char*)GSMArray, FeedData) != Null) return True;
		if(strstr((char*)GSMArray, "ERROR") != Null) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}		

char GSM_CheckSignal()
{
	unsigned char MaximumDelay = 20;
	do
	{	
		char LocalVariable = GSM_TalkBack("AT+CREG?\r\n","OK",2500);
		if(strstr((char*)GSMArray, "0,0") != Null) return False; //Fail To Search Network
		else if(strstr((char*)GSMArray, "0,3") != Null) return False; //Registration Denied
		else if(strstr((char*)GSMArray, "0,4") != Null) return False; //Unknown Err0r
		else if(strstr((char*)GSMArray, "0,1") != Null) return True; //Registered Home NetWork
		else if(strstr((char*)GSMArray, "0,5") != Null) return True; //Registerd Roaming
		//else if(!LocalVariable) return False; //Process Without GSM
		DelayMS(1000);
	}
	while(MaximumDelay --> 0);
	return False;
}								