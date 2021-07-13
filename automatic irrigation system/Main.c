#include <pic.h>
#include <string.h>
#include <stdlib.h>
#include "clock.h"
#include "header.h"
#include "int.h"
#include "timer.h"
#include "lcd.h"
#include "adc.h"
#include "dht11.h"
#include "suart.h"

//definitions of Pump
#define PumpPort PORTD
#define PumpDirection TRISD
#define PumpPin 1

//definitions of Alarm
#define AlarmPort PORTD
#define AlarmDirection TRISD
#define AlarmPin 2

//definitions of SoilSensor
#define Soil 0

//definitions of KeyPad
#define KeyPadPort PORTD
#define KeyPadDirection TRISD
#define SetKey 3
#define MovKey 4
#define IncKey 5
#define DecKey 6
#define EntKey 7

//Variables For SSerialRoutine
unsigned char GSMArray[70], GSMCount;
unsigned char GlobalArray[50];
unsigned char MobileNumber1[15], MobileNumber2[15];

//Variables For Processing
float Temperature, Humidity;
unsigned char SoilSensor, SoilMinimum, SoilMaximum;
char ISPumpOn;

//SSerial Routine For GSM 
void SSerialRoutine()
{
	if(!SSerialReceive)
	{
		GSMArray[GSMCount] = SSerial_Receive();
		GSMCount = (GSMCount < 70 ?GSMCount+1 :0);
	}
	CCP1IF = 0;
}

//GSM Routines
void GSM_Initialize();
void GSM_SendMessage(const char*, const char*);
char GSM_TalkBack(const char*, const char*, unsigned int);

//Keypad Routine
void KeyPad(unsigned char*, unsigned int);

void main()
{
	PumpDirection &=~ (1 << PumpPin);
	AlarmDirection &=~ (1 << AlarmPin);
	KeyPadDirection |= ((1 << SetKey) | (1 << MovKey) | (1 << EntKey) | (1 << IncKey) | (1 << DecKey));
	PumpPort &=~ (1 << PumpPin);
	AlarmPort &=~ (1 << AlarmPin);
	
	LCD_Initialize();
	LCD_Disp(0x80,"AUTO IRRIGA SYST");
	LCD_Disp(0xC0,"USING SOIL MOIST");
	DelayMS(3000); LCDClear();
	
	SoilMinimum = (eeprom_read(0x10) == 0xFF ?0x00 :eeprom_read(0x10));
	SoilMaximum = (eeprom_read(0x11) == 0xFF ?0x00 :eeprom_read(0x11));
	for(char k = 0; k < 10; k++) MobileNumber1[k] = eeprom_read(0x00 + k);
	
	LCD_Disp(0x80,"S.<:"); LCD_Decimal(0x84, SoilMinimum, 3, DEC);
	LCD_Disp(0x88,"S.>:"); LCD_Decimal(0x8C, SoilMaximum, 3, DEC);
	LCD_Disp(0xC0,"NUM:"); LCD_Disp(0xC4, (char*)MobileNumber1); 
	DelayMS(2000); LCDClear();
	
	SSerial_Initialize();
	SSerial_Enable(True);
	EnableInterrupt(CP1INT, SSerialRoutine);
	GSM_Initialize();
	DHT_Initialize();
	
	while(1)
	{
		FetchDHT(&Humidity, &Temperature);
		SoilSensor = MapValue(ReadRawADC8(Soil, 10), 0, 255, 0, 100);
		
		LCD_Disp(0x80,"H:"); LCD_Float(0x82, Humidity, 2, 1, NoSign);
		LCD_Disp(0x87,"T:"); LCD_Float(0x89, Temperature, 2, 1, NoSign);
		LCD_Disp(0xC0,"S:"); LCD_Decimal(0xC2, SoilSensor, 3, DEC);
		LCD_Disp(0xC6,"P:"); 
		
		if(SoilMinimum && SoilSensor < SoilMinimum && !ISPumpOn)
		{
			LCD_Disp(0xC8, "ON ");
			LCD_Disp(0xCC, "SEND");
			PumpPort |= (1 << PumpPin);
			AlarmPort |= (1 << AlarmPin);
			GSM_SendMessage((char*)MobileNumber1, "Pump is Activated in Your Field");
			AlarmPort &=~ (1 << AlarmPin);
			LCD_Disp(0xCC, "    ");
			ISPumpOn = True;
		}
		if(SoilMaximum && SoilSensor > SoilMaximum && ISPumpOn)
		{
			LCD_Disp(0xC8, "OFF");
			LCD_Disp(0xCC, "SEND");
			PumpPort &=~ (1 << PumpPin);
			AlarmPort |= (1 << AlarmPin);
			GSM_SendMessage((char*)MobileNumber1, "Pump is Deactivated in Your Field");
			AlarmPort &=~ (1 << AlarmPin);
			LCD_Disp(0xCC, "    ");
			ISPumpOn = False;
		}
		if(SoilMinimum && SoilMaximum && SoilSensor > SoilMinimum && SoilSensor < SoilMaximum)
		{
			LCD_Disp(0xCC, "    ");
			LCD_Disp(0xC8, "OFF");
			PumpPort &=~ (1 << PumpPin);
			AlarmPort &=~ (1 << AlarmPin);	
			ISPumpOn = False;
		}
		if(!(KeyPadPort & (1 << SetKey)))
		{
			StopTimer();
			while(!(KeyPadPort & (1 << SetKey)));
			char LocalVariable;
			
			LCDClear();
			LCD_Disp(0x80,"ENTER SOIL MINIMUM");
			KeyPad((char*)GlobalArray, 3);
			LocalVariable = atoi((char*)GlobalArray);
			SoilMinimum = (LocalVariable > 100 ?100 :LocalVariable);
			eeprom_write(0x10, SoilMinimum);
			
			LCDClear();
			LCD_Disp(0x80,"ENTER SOIL MAXIMUM");
			KeyPad((char*)GlobalArray, 3);
			LocalVariable = atoi((char*)GlobalArray);
			SoilMaximum = (LocalVariable > 100 ?100 :LocalVariable);
			eeprom_write(0x11, SoilMaximum);
			
			LCDClear();
			LCD_Disp(0x80,"ENTER MOB NUMBER ");
			KeyPad((char*)MobileNumber1, 10);
			for(char k = 0; k < 10; k++)
			eeprom_write(0x00 + k, MobileNumber1[k]);
			LCDClear(); RestartTimer();
		}
		if(!(KeyPadPort & (1 << MovKey)))
		{
			LCDClear(); StopTimer();
			LCD_Disp(0x80,"S.<:"); LCD_Decimal(0x84, SoilMinimum, 3, DEC);
			LCD_Disp(0x88,"S.>:"); LCD_Decimal(0x8C, SoilMaximum, 3, DEC);
			LCD_Disp(0xC0,"NUM:"); LCD_Disp(0xC4, (char*)MobileNumber1); 
			while(!(KeyPadPort & (1 << MovKey)));
			DelayMS(1000); LCDClear(); RestartTimer();
		}	
		if(smemmem((char*)GSMArray, GSMCount, "+91", 2) != Null)
		{
			DelayMS(100); StopTimer();
			if(strstr((char*)GSMArray, "Status") != Null)
			{
				unsigned char LocalArray[10];
				LCDClear(); LCD_Disp(0x80," QUERY RECEIVED ");
				SplitString((char*)GSMArray, (char*)MobileNumber2, "+91", '"');
				LCD_Disp(0xC0,(char*)MobileNumber2); LCD_Disp(0xCC, "SEND");
				memset(GlobalArray, '\0', sizeof(GlobalArray));
				strcat((char*)GlobalArray, "PUMP: "); 
				strcat((char*)GlobalArray, (ISPumpOn ?"ON\n" :"OFF\n"));
				strcat((char*)GlobalArray, "TEMP: "); ftostra(Temperature, 2, 1, (char*)LocalArray, NoSign);
				strcat((char*)GlobalArray, (char*)LocalArray); strcat((char*)GlobalArray, "\n");
				strcat((char*)GlobalArray, "HUMI: "); ftostra(Humidity, 2, 1, (char*)LocalArray, NoSign);
				strcat((char*)GlobalArray, (char*)LocalArray); strcat((char*)GlobalArray, "\n");
				strcat((char*)GlobalArray, "SOIL: "); dtoa(SoilSensor, (char*)LocalArray, 3, DEC);
				strcat((char*)GlobalArray, (char*)LocalArray); GSM_SendMessage((char*)MobileNumber2, (char*)GlobalArray);
				memset(MobileNumber2, '\0', sizeof(MobileNumber2)); memset(GlobalArray, '\0', sizeof(GlobalArray));
				LCDClear();
			}
			memset(GSMArray, '\0', sizeof(GSMArray));
			GSMCount = 0; RestartTimer();
		}
	}
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
	GSMCount = 0; SSerial_Write(SendData);
	do
	{
		if(strstr((char*)GSMArray, FeedData) != Null) return True;
		if(strstr((char*)GSMArray, "ERROR") != Null) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
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
			while(!(KeyPadPort & (1 << MovKey)));
			__delay_ms(100); __delay_ms(100);
			if(++MaximumMove > (RequireDigit - 1)) MaximumMove = 0;
		}
		if(!(KeyPadPort & (1 << SetKey)))
		{
			LCD_Write(0xC0 + MaximumMove, ' ');
			while(!(KeyPadPort & (1 << SetKey)));
			if(--MaximumMove > (RequireDigit - 1))
			MaximumMove = (RequireDigit - 1);
		}	 
		RequireArray[MaximumMove] = InnerCount + '0';
		LCD_Write(0xC0 + MaximumMove, RequireArray[MaximumMove]);
	}			
	while(!(KeyPadPort & (1 << EntKey)));
	LCD_Command(0x0C);	
}