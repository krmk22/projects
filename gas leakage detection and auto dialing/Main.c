#include <pic.h>
#include <string.h>
#include <stdlib.h>

#include "clock.h"
#include "header.h"
#include "int.h"
#include "lcd.h"
#include "adc.h"
#include "suart.h"
#include "uart.h"

//Alarm Pin Config
#define AlarmPort PORTD
#define AlarmDirection TRISD
#define AlarmPin 0

//Exhaust Fan Pin Config
#define FanPort PORTD
#define FanDirection TRISD
#define FanPin 1

//MP3 Module Busy Config
#define BusyPort PORTD
#define BusyDirection TRISD
#define BusyPin 2

//Keypad Pin Config
#define KeyPadPort PORTD
#define KeyPadDirection TRISD
#define SetKey 3
#define MovKey 4
#define IncKey 5
#define DecKey 6
#define EntKey 7

//Gas Sensor Analog Config
#define GSensor 0

//MP3 Module Commands
#define Volume 0x06
#define Track 0x03

unsigned char GasSensor, GasValue;
unsigned char GSMArray[75], GSMCount;
unsigned char MobileNumber1[12], MobileNumber2[12], MobileNumber3[12];
bit ISLeakageInformed;

//SerialRoutine For GSM
void SerialRoutine()
{
	GSMArray[GSMCount] = RCREG;
	GSMCount = (GSMCount < 75 ?GSMCount+1 :0);
	RCIF = 0;
}	

//GSMRoutines
void GSM_Initialize();
void GSM_SendMessage(const char*, const char*);
char GSM_TalkBack(const char*, const char*, unsigned int);
char GSM_Outgoing(const char*);
char GSM_Incoming();

//KeypadRoutines
void KeyPad(unsigned char*, unsigned int);

//MP3 Routine
void SendCommand(unsigned char, unsigned int);

//Auto Dial Config
void AutoDial(const char*);

/*Voice List
001 -> 1 
002 -> 2
003 -> 3
004 -> 4
005 -> 5
006 -> 6
007 -> 7
008 -> 8
009 -> 9
010 -> 0 // A

011 -> Gas Leakage Detected //B
012 -> Gas Sensor Normal //C
012 -> Current Leakage Level is //D 
*/

void main()
{
	AlarmDirection &=~ (1 << AlarmPin);
	FanDirection &=~ (1 << FanPin);
	BusyDirection |= (1 << BusyPin);
	KeyPadDirection |= ((1 << SetKey) | (1 << MovKey) | (1 << IncKey) | (1 << DecKey) | (1 << EntKey));
	
	AlarmPort &=~ (1 << AlarmPin);
	FanPort &=~ (1 << FanPin);
	
	LCD_Initialize();
	LCD_Disp(0x80,"GAS LEAKAGE DETE");
	LCD_Disp(0xC0,"AUTO DIALING SYS");
	DelayMS(3000); LCDClear();
	
	for(char k = 0; k < 10; k++)
	{
		MobileNumber1[k] = eeprom_read(0x10 + k);
		LCD_Write(0x83 + k, MobileNumber1[k]);
		MobileNumber2[k] = eeprom_read(0x20 + k);
		LCD_Write(0xC3 + k, MobileNumber2[k]);
	}
	DelayMS(1000); LCDClear();
	
	GasValue = (eeprom_read(0x00) == 0xFF ?100 :eeprom_read(0x00));
	LCD_Disp(0x80,"SET VAL:"); LCD_Decimal(0x88, GasValue, 3, DEC);
	
	do
	{
		GasSensor = MapValue(ReadRawADC8(GSensor, 10), 0, 255, 0, 100);
		LCD_Disp(0xC0,"CUR VAL:");
		LCD_Decimal(0xC8, GasSensor, 3, DEC);
	}
	while(GasSensor > GasValue);	
	DelayMS(1000); LCDClear();
	
	SSerial_Initialize();
	Serial_Initialize();
	Serial_Enable(True);
	EnableInterrupt(RXRINT, (char*)SerialRoutine);
	GSM_Initialize(); 
	SendCommand(Volume, 0x1E);
	LCD_Disp(0x88,"NORMAL");
	
	while(1)
	{
		GasSensor = MapValue(ReadRawADC8(GSensor, 10), 0, 255, 0, 100);
		LCD_Disp(0x80,"GAS:"); LCD_Decimal(0x84, GasSensor, 3, DEC); 
		
		if(GasSensor > GasValue && !ISLeakageInformed)
		{
			LCD_Disp(0x88,"SENSE ");
			AlarmPort |= (1 << AlarmPin);
			FanPort |= (1 << FanPin);
			LCD_Disp(0xC0, (char*)MobileNumber1);
			AutoDial((char*)MobileNumber1);
			LCD_Disp(0xC0, (char*)MobileNumber2);
			AutoDial((char*)MobileNumber2);
			ISLeakageInformed = True;
		}
		if(GasSensor < GasValue && ISLeakageInformed)
		{
			LCD_Disp(0x88,"NORMAL");
			AlarmPort &=~ (1 << AlarmPin);
			FanPort &=~ (1 << FanPin);
			ISLeakageInformed = False;
		}
		if(GSM_Incoming())
		{
			memset((char*)MobileNumber3, '\0', sizeof(MobileNumber3));
			SplitString((char*)GSMArray, (char*)MobileNumber3, "+91", '"');
			LCD_Disp(0xC0, (char*)MobileNumber3); DelayMS(3000);
			AutoDial((char*)MobileNumber3);
		}
		if(!(KeyPadPort & (1 << SetKey)))
		{
			while(!(KeyPadPort & (1 << SetKey)));
			
			LCDClear();
			LCD_Disp(0x80,"ENTER GAS LEVEL ");
			memset(MobileNumber3, '\0', sizeof(MobileNumber3));
			KeyPad((char*)MobileNumber3, 3);
			GasValue = atoi((char*)MobileNumber3);
			GasValue = (!GasValue || GasValue > 100 ?0 :GasValue);
			eeprom_write(0x00, GasValue); LCDClear();
			
			LCD_Disp(0x80,"ENTER NUMBER 1");
			KeyPad((char*)MobileNumber1, 10);
			for(char k = 0; k < 10; k++) eeprom_write(0x10 + k, MobileNumber1[k]);
			LCDClear();
			
			LCD_Disp(0x80,"ENTER NUMBER 2");
			KeyPad((char*)MobileNumber2, 10);
			for(char k = 0; k < 10; k++) eeprom_write(0x20 + k, MobileNumber2[k]);
			LCDClear();
		}
		if(!(KeyPadPort & (1 << MovKey)))
		{
			LCDClear();
			LCD_Disp(0x80, (char*)MobileNumber1);
			LCD_Disp(0xC0, (char*)MobileNumber2);
			LCD_Decimal(0x8B, GasValue, 3, DEC);
			while(!(KeyPadPort & (1 << MovKey)));
			DelayMS(1000); LCDClear();
		}		
	}
}

void KeyPad(unsigned char *RequireArray, unsigned int RequireDigit)
{
	unsigned char MaximumMove = 0, InnerCount = 0;
	LCD_Command(0x0E);
	
	while(KeyPadPort & (1 << EntKey))
	{
		if(!(KeyPadPort & (1 << IncKey)))
		{
			while(!(KeyPadPort & (1 << IncKey)));
			if(++InnerCount > 9) InnerCount = 0;
		}
		if(!(KeyPadPort & (1 << DecKey)))
		{
			while(!(KeyPadPort & (1 << DecKey)));
			if(--InnerCount > 9) InnerCount = 9;
		}
		if(!(KeyPadPort & (1 << MovKey)))
		{
			while(!(KeyPadPort & (1 << MovKey))); DelayMS(200);
			if(++MaximumMove > (RequireDigit - 1)) MaximumMove = 0;
		}
		if(!(KeyPadPort & (1 << SetKey)))
		{
			LCD_Write(0xC0 + MaximumMove, ' ');
			while(!(KeyPadPort & (1 << SetKey))); DelayMS(200);
			if(--MaximumMove > (RequireDigit - 1)) MaximumMove = (RequireDigit - 1);
		}	
		RequireArray[MaximumMove] = InnerCount + '0';
		LCD_Write(0xC0 + MaximumMove, RequireArray[MaximumMove]);
	}			
	while(!(KeyPadPort & (1 << EntKey)));
	LCD_Command(0x0C);	
}

void SendCommand(unsigned char Command, unsigned int Data)
{
	unsigned char SendBytes[10];
	SendBytes[0] = 0x7E;
	SendBytes[1] = 0xFF;
	SendBytes[2] = 0x06;
	SendBytes[3] = Command;
	SendBytes[4] = 0x00;
	SendBytes[5] = (unsigned char)((Data >> 8) & 0xFF);
	SendBytes[6] = (unsigned char)(Data & 0xFF);
	unsigned int CSum = -(SendBytes[1] + SendBytes[2] + 
						  SendBytes[3] + SendBytes[4] +
						  SendBytes[5] + SendBytes[6]);
	SendBytes[7] = (unsigned char)((CSum >> 8) & 0xFF);
	SendBytes[8] = (unsigned char)(CSum & 0xFF);
	SendBytes[9] = 0xEF;
	SSerial_Writeln((char*)SendBytes, 10); DelayMS(100);
}	

void GSM_Initialize()
{
	if(GSM_TalkBack("AT\r\n","OK",2500)) LCD_Disp(0x80,"GSM FOUND ");
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
	
	if(GSM_TalkBack("AT+CNMI=2,2,0,0,0\r\n","OK",2500)) LCD_Disp(0x80,"TERMINAL READY");
	else LCD_Disp(0x80,"TERMINAL ERROR"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CLIP=1\r\n","OK",2500)) LCD_Disp(0x80,"CALLER ID READY ");
	else LCD_Disp(0x80,"CALLER ID ERROR"); DelayMS(1000); LCDClear();
}

void GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
	SSerial_Write("AT+CMGS=\"+91"); SSerial_Write(MobileNumber);
	SSerial_Write("\"\r\n"); GSM_TalkBack("",">",2500);
	SSerial_Write(SendData); DelayMS(100); SSerial_Send(0x1A);
	GSM_TalkBack("","OK",5000); GSMCount = 0;
	memset(GSMArray, '\0', sizeof(GSMArray));
}	

char GSM_TalkBack(const char *SendData, const char *FeedData, unsigned int TimeDelay)
{
	memset(GSMArray, '\0', sizeof(GSMArray));	
	GSMCount = 0; Serial_Write(SendData);
	do
	{
		if(strstr((char*)GSMArray, FeedData) != Null) return True;
		if(strstr((char*)GSMArray, "ERROR") != Null) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}

char GSM_Outgoing(const char *MobileNumber)
{
	unsigned char MaximumDelay = 20;
	Serial_Write("ATD+91"); Serial_Write(MobileNumber);
	GSM_TalkBack(";\r\n","OK", 2500); DelayMS(1000);
	memset(GSMArray, '\0', sizeof(GSMArray));
	do
	{
		GSM_TalkBack("AT+CLCC\r\n","OK",2500);
		if(strstr((char*)GSMArray, "0,0,0,0") != Null)
		{
			LCD_Disp(0xCB, "ATTND");
			return True;
		}
		else if(strstr((char*)GSMArray, "0,6,0,0") != Null)
		{
			LCD_Disp(0xCB, "DISCN");
			return False;
		}
		else if(strstr((char*)GSMArray, "0,2,0,0") != Null) LCD_Disp(0xCB,"DIAL ");
		else if(strstr((char*)GSMArray, "0,3,0,0") != Null) LCD_Disp(0xCB,"RING ");		
		DelayMS(1000);	
	}
	while(MaximumDelay --> 0);
	return False;
}						

char GSM_Incoming()
{
	if(smemmem((char*)GSMArray, GSMCount, "RING", 4) != Null)
	{
		if(strstr((char*)GSMArray, "+91") != Null)
		{
			DelayMS(100);
			Serial_Write("ATH\r\n");
			return True;
		}
	}
	return False;
}			

void AutoDial(const char *MobileNumber)
{
	unsigned char LocalArray[5];
	if(GSM_Outgoing(MobileNumber))
	{
		if(GasSensor > GasValue) SendCommand(Track, 0x0B);
		else SendCommand(Track, 0x0C);
		while(!(BusyPort & (1 << BusyPin)));
		SendCommand(Track, 0x0D); while(!(BusyPort & (1 << BusyPin)));
		dtoa(GasSensor, (char*)LocalArray, 3, DEC);
		for(char k = 0; LocalArray[k] != '\0'; k++)
		{
			if(LocalArray[k] == '0') SendCommand(Track, 0x0A);
			else SendCommand(Track, LocalArray[k] - '0');
			while(!(BusyPort & (1 << BusyPin)));
		}
	}
	GSM_TalkBack("ATH\r\n","OK",3000);
	LCD_Disp(0xC0,"                ");
}			