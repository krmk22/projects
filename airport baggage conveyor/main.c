#include <pic.h>
#include <string.h>

#include "clock.h"
#include "header.h"
#include "lcd.h"
#include "int.h"
#include "timer.h"
#include "sim800l.h"
#include "suart.h"
#include "uart.h"

#ifndef CardNumbers
#define CardNumbers
#define Card1 "2600A416A135"
#define Card2 "2600A41E31AD"
#define Card3 "2600A4466EAA"
#define Card4 "2600A473B544"
#define Card5 "2600A3E47F1E"
#endif

#ifndef MobileNumbers
#define MobileNumbers
#define MobileNumber1 "6382722861" // --> User1
#define MobileNumber2 "6381733133" // --> User2
#define MobileNumber3 "9080148941" // --> User3 
#define MobileNumber4 "7010083029" // --> User4
#define MobileNumber5 "9486778982" // --> User5
#endif

#ifndef MP3ModuleCommands
#define MP3ModuleCommands
#define Volume 0x06
#define Track 0x03
#endif

#ifndef SensorConfig
#define SensorConfig
#define SensorPort PORTD
#define SensorDirection TRISD
#define SensorPin 0
#endif

#ifndef MotorConfig
#define MotorConfig
#define MotorPort PORTD
#define MotorDirection TRISD
#define MotorPin 1
#endif

#ifndef BusyConfig
#define BusyConfig
#define BusyPort PORTD
#define BusyDirection TRISD
#define BusyPin 2
#endif

/*
000 -> NULL
001 -> USER1
002 -> USER2
003 -> USER3
004 -> USER4
005 -> USER5
006 -> Youe Baggage is Awaited
007 -> Please Collect Your Bag
008 -> Project Title
009 -> Put your bag into the conveyor
*/

unsigned char SerialArray[15], SerialCount;
unsigned char LastBaggage;

//char GSM_Initialize();

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
	Serial_Writeln((char*)SendBytes, 10); DelayMS(200);
}	

void SerialRoutine()
{
	SerialArray[SerialCount] = RCREG;
	SerialCount = (SerialCount < 15 ?SerialCount+1 :0);
	RCIF = 0;
}	

void main()
{
	SensorDirection |= (1 << SensorPin);
	BusyDirection |= (1 << BusyPin);
	
	MotorDirection &=~ (1 << MotorPin);
	MotorPort &=~ (1 << MotorPin);
	
	LCD_Initialize();
	LCD_Disp(0x80,"AIRPORT BAGGAGE "); 
	LCD_Disp(0xC0,"   CONVEYOR     ");
	DelayMS(3000); LCDClear();
	
	Serial_Initialize();
	EnableInterrupt(RXRINT, SerialRoutine);
		
	SendCommand(Volume, 0x1E);
    SendCommand(Track, 0x08);
	while(!(BusyPort & (1 << BusyPin)));
	
	/*
	switch(GSM_Initialize())
	{
		case NO_GSM:
		LCD_Disp(0x80,"GSM NOT FOUND...");
		break;
		case NO_SIM:
		LCD_Disp(0x80,"NO SIM FOUND....");
		break;
		case NO_NET:
		LCD_Disp(0x80,"N/W ERROR....");
		break;
		case GSM_OK:
		LCD_Disp(0x80,"GSM INITIALIZED");
		break;
		default:
		LCD_Disp(0x80,"UNKNOWN ERROR..");
		break;
	}
	DelayMS(1000); LCDClear();	
	*/
	
	GSM_Initialize();
	if(GSM_TalkBack("AT\r\n","OK",2500)) LCD_Disp(0x80,"GSM FOUND...");
	else LCD_Disp(0x80,"GSM NOT FOUND"); DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("ATE0\r\n","OK",2500)) LCD_Disp(0x80,"ECHO CLEARED");
	else LCD_Disp(0x80,"ECHO NOT CLEAR"); DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CSMINS?\r\n","0,1",2500)) LCD_Disp(0x80,"SIM FOUND");
	else LCD_Disp(0x80,"SIM NOT FOUND"); DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CPIN?\r\n","READY",2500)) LCD_Disp(0x80,"SCURE ACCESS");
	else LCD_Disp(0x80,"SIM LOCK"); DelayMS(500); LCDClear();
	
	if(GSM_CheckSignal()) LCD_Disp(0x80,"SIM TOWERED");
	else LCD_Disp(0x80,"SIM NOT TOWERED"); DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CMGF=1\r\n","OK",2500)) LCD_Disp(0x80,"MESSAGE OK");
	else LCD_Disp(0x80,"MESSAGE ERROR"); DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CNMI=2,2,0,0,0\r\n","OK",2500)) LCD_Disp(0x80,"CALL OK");
	else LCD_Disp(0x80,"CALL ERROR"); DelayMS(500); LCDClear();
	
	SendCommand(Track, 0x09);
	while(!(BusyPort & (1 << BusyPin)));
	MotorPort |= (1 << MotorPin);
	Serial_Enable(True); 
	
	while(1)
	{
		LCD_Decimal(0xCF,  MilliTicks / 1000, 1, DEC);
		
		if(strstr((char*)SerialArray, Card1) != '\0' && SerialCount > 11)
		{
			LCD_Disp(0x80,"BAGGAGE FOUND...");
			memset(SerialArray, '\0', sizeof(SerialArray));
			LastBaggage = 1; SerialCount = 0; Serial_Enable(False);
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, 0x01);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x06);
			while(!(BusyPort & (1 << BusyPin)));
			MotorPort |= (1 << MotorPin);
		}
		else if(strstr((char*)SerialArray, Card2) != '\0' && SerialCount > 11)
		{
			LCD_Disp(0x80,"BAGGAGE FOUND...");
			memset(SerialArray, '\0', sizeof(SerialArray));
			LastBaggage = 2; SerialCount = 0; Serial_Enable(False);
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, 0x02);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x06);
			while(!(BusyPort & (1 << BusyPin)));
			MotorPort |= (1 << MotorPin);
		}
		else if(strstr((char*)SerialArray, Card3) != '\0' && SerialCount > 11)
		{
			LCD_Disp(0x80,"BAGGAGE FOUND...");
			memset(SerialArray, '\0', sizeof(SerialArray));
			LastBaggage = 3; SerialCount = 0; Serial_Enable(False);
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, 0x03);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x06);
			while(!(BusyPort & (1 << BusyPin)));
			MotorPort |= (1 << MotorPin);
		}
		else if(strstr((char*)SerialArray, Card4) != '\0' && SerialCount > 11)
		{
			LCD_Disp(0x80,"BAGGAGE FOUND...");
			memset(SerialArray, '\0', sizeof(SerialArray));
			LastBaggage = 4; SerialCount = 0; Serial_Enable(False);
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, 0x04);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x06);
			while(!(BusyPort & (1 << BusyPin)));
			MotorPort |= (1 << MotorPin);
		}
		else if(strstr((char*)SerialArray, Card5) != '\0' && SerialCount > 11)
		{
			LCD_Disp(0x80,"BAGGAGE FOUND...");
			memset(SerialArray, '\0', sizeof(SerialArray));
			LastBaggage = 5; SerialCount = 0; Serial_Enable(False);
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, 0x05);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x06);
			while(!(BusyPort & (1 << BusyPin)));
			MotorPort |= (1 << MotorPin);
		}	
		else if((SensorPort & (1 << SensorPin)) && !MilliTicks && LastBaggage)
		{
			LCD_Disp(0xC0,"COLLECT BAGGAGE ");
			MotorPort &=~ (1 << MotorPin);
			SendCommand(Track, LastBaggage);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x07);
			while(!(BusyPort & (1 << BusyPin)));
			StartTimer(); MilliTicks = 1;
		}
		else if((SensorPort & (1 << SensorPin)) && MilliTicks > 10000)
		{
			SendCommand(Track, LastBaggage);
			while(!(BusyPort & (1 << BusyPin)));
			SendCommand(Track, 0x07);
			while(!(BusyPort & (1 << BusyPin)));
			StopTimer(); LastTicks = 1;
			
			switch(LastBaggage)
			{
				case 1: GSM_SendMessage(MobileNumber1, "Please Collect Your Baggage"); break;
				case 2: GSM_SendMessage(MobileNumber2, "Please Collect Your Baggage"); break;
				case 3: GSM_SendMessage(MobileNumber3, "Please Collect Your Baggage"); break;
				case 4: GSM_SendMessage(MobileNumber4, "Please Collect Your Baggage"); break;
				case 5: GSM_SendMessage(MobileNumber5, "Please Collect Your Baggage"); break;
				default: break;
			}
			RestartTimer();	
		}
		else if(!(SensorPort & (1 << SensorPin)) && MilliTicks)
		{
			StopTimer(); MilliTicks = 0;
			switch(LastBaggage)
			{
				case 1: GSM_SendMessage(MobileNumber1, "Your Baggage Collected..."); break;
				case 2: GSM_SendMessage(MobileNumber2, "Your Baggage Collected..."); break;
				case 3: GSM_SendMessage(MobileNumber3, "Your Baggage Collected..."); break;
				case 4: GSM_SendMessage(MobileNumber4, "Your Baggage Collected..."); break;
				case 5: GSM_SendMessage(MobileNumber5, "Your Baggage Collected..."); break;
				default: break;
			}	
			memset(SerialArray, '\0', sizeof(SerialArray));
			MotorPort |= (1 << MotorPin);
			SerialCount = 0; LCDClear(); Serial_Enable(True);
		}
	}
}
				
/*
char GSM_Initialize()
{
	SSerial_Initialize();
	SSerial_Enable(True);
	EnableInterrupt(CP1INT, GSMRoutine);
	
	GIE = PEIE = 1; LCDClear(); GSMCount = 0;
	memset(GSMArray, '\0', sizeof(GSMArray));
	
	if(GSM_TalkBack("AT\r\n","OK",2500)) LCD_Disp(0x80,"GSM FOUND...");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("ATE0\r\n","OK",2500)) LCD_Disp(0x80,"ECHO CLEARED");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CSMINS?\r\n","0,1",2500)) LCD_Disp(0x80,"SIM FOUND...");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CPIN?\r\n","READY",2500)) LCD_Disp(0x80,"SECURE ACCESSED");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_CheckSignal()) LCD_Disp(0x80,"GSM TOWERED...");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CMGF=1\r\n","OK",2500)) LCD_Disp(0x80,"MESSAGE READY");
	else return False; DelayMS(500); LCDClear();
	
	if(GSM_TalkBack("AT+CLIP=1\r\n","OK",2500)) LCD_Disp(0x80,"CALLER ID READY");
	else return False; DelayMS(500); LCDClear();
	
	return True;
}
*/	
			