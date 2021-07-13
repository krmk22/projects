#include <pic.h>
#include <string.h>
#include "clock.h"
#include "header.h"
#include "timer.h"
#include "adc.h"
#include "int.h"
#include "lcd.h"
#include "uart.h"

#ifndef RelayConfig
#define RelayConfig
#define RelayPort PORTC
#define RelayDirection TRISC
#define RelayPin 0
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define AlarmPort PORTC
#define AlarmDirection TRISC
#define AlarmPin 1
#endif

#ifndef KeypadConfig
#define KeypadConfig
#define KeypadPort PORTD
#define KeypadDirection TRISD
#define SetKey 0
#define MovKey 1
#define IncKey 2
#define DecKey 3
#define EntKey 4
#endif

#ifndef ADCConfig
#define ADCConfig
#define VoltagePin 0
#define CurrentPin 1
#endif

char GSM_TalkBack(const char*, const char*, unsigned int);
void GSM_Initialize();
void GSM_SendMessage(const char*, const char*);
void KeyPadRoutine();

unsigned int PulseCount;
unsigned char Voltage;
float Current;

unsigned char PropNumber[15];
unsigned char AuthNumber[15];
unsigned char SerialArray[96], SerialCount; //<-- Maximum

unsigned long int TimerDuration;
unsigned int RoutinePulseCount;
unsigned char ExpectedPulse;
float Power;

void SerialRoutine()
{
	SerialArray[SerialCount] = RCREG;
	SerialCount = (SerialCount < 96 ?SerialCount + 1 :SerialCount);
	RCIF = 0;
}

void ExternalRoutine()
{
	PulseCount++;
	RoutinePulseCount++;
	INTF = 0;
}	

void main()
{
	RelayDirection &=~ (1 << RelayPin);
	AlarmDirection &=~ (1 << AlarmPin);
	KeypadDirection |= ((1 << SetKey) | (1 << MovKey) | (1 << IncKey) | (1 << DecKey) | (1 << EntKey));
	
	RelayPort &=~ (1 << RelayPin);
	AlarmPort &=~ (1 << AlarmPin);
	
	LCD_Initialize();
	LCD_Disp(0x80,"GSM BASED POWER ");
	LCD_Disp(0xC0,"THEFT IDENTIFIER");
	DelayMS(3000); LCDClear();
	
	LCD_Disp(0x80,"+91");
	LCD_Disp(0xC0,"+91");
	
	for(char k = 0; k < 10; k++)
	{
		PropNumber[k] = eeprom_read(0x00 + k);
		LCD_Write(0x83 + k, PropNumber[k]);
		AuthNumber[k] = eeprom_read(0x10 + k);
		LCD_Write(0xC3 + k, AuthNumber[k]);
	}
	DelayMS(1000); LCDClear();
	
	Serial_Initialize(); GIE = PEIE = 1;
	EnableInterrupt(RXRINT, (char*)SerialRoutine);
	Serial_Enable(True);
	
	GSM_Initialize();
	INTE = 1; INTEDG = 1;
	EnableInterrupt(EXTINT, (char*)ExternalRoutine);
	StartTimer();
	
	RelayPort |= (1 << RelayPin); DelayMS(100);
	PulseCount = 0; RoutinePulseCount = 0;
	TimerDuration = 60000;
	
	while(1)
	{
		Voltage = ReadRawADC8(VoltagePin, 25);
		Current = ReadRawADC8(CurrentPin, 10) * 0.01F;
		
		LCD_Disp(0x80,"PulseCount:");
		LCD_Decimal(0x8B, PulseCount, 3, DEC);
		LCD_Disp(0xC0,"V:");
		LCD_Decimal(0xC2, Voltage, 3, DEC);
		LCD_Disp(0xC6,"C:");
		LCD_Float(0xC8, Current, 1, 2, NoSign);
		LCD_Decimal(0xCE, MilliTicks / 1000, 2, DEC);
		
		if(smemmem((char*)SerialArray, SerialCount, "+91", 3) != '\0')
		{	
			DelayMS(100);
			if(strstr((char*)SerialArray, "PowerOn") != '\0')
			{
				RelayPort |= (1 << RelayPin);
			}
			else if(strstr((char*)SerialArray, "PowerOff") != '\0')
			{
				RelayPort &=~ (1 << RelayPin);
			}
			memset((char*)SerialArray, '\0', sizeof(SerialArray));
			SerialCount = 0;
		}
		if(MilliTicks > TimerDuration)
		{
			LCDClear();
			Power = Voltage * Current;
			ExpectedPulse = (unsigned)((3200 * Power * 60) / (1000 * 3600)) | 1;
			LCD_Disp(0x80,"E.P:"); LCD_Decimal(0x84, ExpectedPulse, 2, DEC);
			LCD_Disp(0x87,"C.P:"); LCD_Decimal(0x8B, RoutinePulseCount, 2, DEC);
			
			if((RoutinePulseCount && !Current) || (RoutinePulseCount > ExpectedPulse))
			{
				LCD_Disp(0xC0,"POWER THEFT DETE");
				StopTimer();
				AlarmPort |= (1 << AlarmPin);
				GSM_SendMessage((char*)PropNumber, "Power Theft Detected At Your Meter");
				GSM_SendMessage((char*)AuthNumber, "Power Theft Detected At S.No: 308536");
				AlarmPort &=~ (1 << AlarmPin); 
				RestartTimer();
			}
			RoutinePulseCount = 0;
			TimerDuration = MilliTicks + 60000;
		}	
		/*
		if(!Current && FirstOccurancePulse && LastOccurancePulse)
		{
			AlarmPort |= (1 << AlarmPin); StopTimer();
			GSM_SendMessage((char*)PropNumber, "Power Theft Detected At Your Meter");
			GSM_SendMessage((char*)AuthNumber, "Power Theft Detected At S.No: 308536");
			LastOccurancePulse = FirstOccurancePulse = 0; 
			AlarmPort &=~ (1 << AlarmPin); RestartTimer();
		}
		*/
		if(!(KeypadPort & (1 << SetKey)))
		{
			while(!(KeypadPort & (1 << SetKey)));
			KeyPadRoutine(); LCDClear();
		}
		if(!(KeypadPort & (1 << MovKey)))
		{
			LCDClear();
			LCD_Disp(0x80, (char*)PropNumber);
			LCD_Disp(0xC0, (char*)AuthNumber);
			while(!(KeypadPort & (1 << MovKey)));
			DelayMS(1000); LCDClear();
		}	
	}
}

char GSM_TalkBack(const char *SendData, const char *FeedData, unsigned int TimeDelay)
{
	memset(SerialArray, '\0', sizeof(SerialArray));	
	SerialCount = 0; Serial_Write(SendData);
	do
	{
		if(strstr((char*)SerialArray, FeedData) != Null) return True;
		if(strstr((char*)SerialArray, "ERROR") != Null) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}

void GSM_SendMessage(const char *MobileNumber, const char *SendData)
{
	Serial_Write("AT+CMGS=\"+91"); Serial_Write(MobileNumber);
	Serial_Write("\"\r\n"); GSM_TalkBack("",">",2500);
	Serial_Write(SendData); DelayMS(100); Serial_Send(0x1A);
	GSM_TalkBack("","OK",5000); SerialCount = 0;
	memset(SerialArray, '\0', sizeof(SerialArray));
}

void GSM_Initialize()
{
	if(GSM_TalkBack("AT\r\n","OK",2500)) LCD_Disp(0x80,"GSM FOUND...");
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
	else LCD_Disp(0x80,"N/W ERROR.."); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CMGF=1\r\n","OK",2500)) LCD_Disp(0x80,"MESSAGE READY");
	else LCD_Disp(0x80,"MESSAGE ERROR"); DelayMS(1000); LCDClear();
	
	if(GSM_TalkBack("AT+CNMI=2,2,0,0,0\r\n","OK",2500)) LCD_Disp(0x80,"CALL READY");
	else LCD_Disp(0x80,"CALL ERROR"); DelayMS(1000); LCDClear();
}

void KeyPadRoutine()
{
	memset(PropNumber, '\0', sizeof(PropNumber));
	memset(AuthNumber, '\0', sizeof(AuthNumber));
	
	LCDClear(); 
	LCD_Disp(0x80," ENTER NUMBER 1 ");
	LCD_Command(0x0E);
	
	unsigned char KeyCount = 0, KeyMove = 0;
	while(KeypadPort & (1 << EntKey))
	{
		if(!(KeypadPort & (1 << IncKey)))
		{
			while(!(KeypadPort & (1 << IncKey)));
			if(++KeyCount > 9) KeyCount = 0;
		}
		if(!(KeypadPort & (1 << DecKey)))
		{
			while(!(KeypadPort & (1 << DecKey)));
			if(--KeyCount > 9) KeyCount = 9;
		}
		if(!(KeypadPort & (1 << MovKey))) 
		{
			while(!(KeypadPort & (1 << MovKey)));
			if(++KeyMove > 9) KeyMove = 0;
		}	
		LCD_Write(0xC0 + KeyMove, KeyCount + '0');
		PropNumber[KeyMove] = KeyCount + '0';
	}
	while(!(KeypadPort & (1 << EntKey)));
	
	LCDClear();
	LCD_Disp(0x80," ENTER NUMBER 2 ");
	
	KeyCount = 0; KeyMove = 0;
	while(KeypadPort & (1 << EntKey))
	{
		if(!(KeypadPort & (1 << IncKey)))
		{
			while(!(KeypadPort & (1 << IncKey)));
			if(++KeyCount > 9) KeyCount = 0;
		}
		if(!(KeypadPort & (1 << DecKey)))	
		{
			while(!(KeypadPort & (1 << DecKey)));
			if(--KeyCount > 9) KeyCount = 9;
		}
		if(!(KeypadPort & (1 << MovKey))) 
		{
			while(!(KeypadPort & (1 << MovKey)));
			if(++KeyMove > 9) KeyMove = 0;
		}	
		LCD_Write(0xC0 + KeyMove, KeyCount + '0');
		AuthNumber[KeyMove] = KeyCount + '0';
	}
	while(!(KeypadPort & (1 << EntKey)));
	LCD_Command(0x0C); LCDClear();
	
	LCD_Disp(0x80,"+91");
	LCD_Disp(0xC0,"+91");
	
	for(char k = 0; k < 10; k++)
	{
		eeprom_write(0x00 + k, PropNumber[k]);
		eeprom_write(0x10 + k, AuthNumber[k]);
		LCD_Write(0x83 + k, eeprom_read(0x00 + k));
		LCD_Write(0xC3 + k, eeprom_read(0x10 + k));
	}
	DelayMS(1000); LCDClear(); 
}	