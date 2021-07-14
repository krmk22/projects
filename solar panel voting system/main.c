#include <htc.h>
#include <string.h>

__CONFIG(1, HSPLL);
__CONFIG(2, PWRTEN & BOREN & WDTDIS);
__CONFIG(3, MCLREN & PBDIGITAL & CCP2RC1);
__CONFIG(4, DEBUGDIS & LVPDIS & STVREN & XINSTDIS);

#include "gpio.h"
#include "utils.h"
#include "lcd.h"
#include "keypad.h"
#include "rc522.h"
#include "esp8266.h"
#include "mqtt.h"
#include "uart.h"
#include "suart.h"
#include "r307.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define RS		 PB5
#define EN		 PB4
#define D4		 PB3
#define D5		 PB2
#define D6		 PB1
#define D7		 PB0
#endif

#ifndef KeypadConfig
#define KeypadConfig
#define KeySET	 PD0
#define KeyMOV	 PD1
#define KeyENT   PD2
#define KeyINC   PD3
#define KeyDEC	 PD4
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm	PD5
#endif

#ifndef RFIDConfig
#define RFIDConfig
#define SS		 PC2
#define SCK		 PC3
#define MOSI	 PC4
#define MISO	 PC5
#define Interval 100
#endif

#ifndef ESPConfig
#define ESPConfig
#define ESPTransmitter 	PC6
#define ESPReceiver	   	PC7
#define ESPBaudRate    	115200
#endif

#ifndef FPConfig
#define FPConfig
#define FPTransmitter	PC0
#define FPReceiver		PC1
#define FPTouch			PD6
#define FPBaudRate		57600
#endif

#ifndef RFIDCard
#define RFIDCard
#define USER1 "49338E996D"
#define USER2 "2923B8992B" 
#define USER3 "E9318699C7"
#define USER4 "13CA2102FA"
#define USER5 "E30BD20238"
#define USER6 "63CAAD0307"
#define USER7 "F6D8B91384"
#define USER8 "C324920277"
#define USER9 "49394DB885"
#define USERA "89B441B8C4"
#define USERX "A99F6FD58C"
#endif

extern unsigned char TagArray[16];
unsigned char CardDetected, LastDetectedKey;
unsigned char MatchingScore, FingerID, ISValidCard;
unsigned char Party1, Party2, TotalCounts;
unsigned int KeyDebounceTime, VoterList;

unsigned char ReturnCard()
{
	if(strstr((char*)TagArray, USER1) != '\0' && !(VoterList & (1 << 1))) return 1;
	else if(strstr((char*)TagArray, USER2) != '\0' && !(VoterList & (1 << 2))) return 2;
	else if(strstr((char*)TagArray, USER3) != '\0' && !(VoterList & (1 << 3))) return 3;
	else if(strstr((char*)TagArray, USER4) != '\0' && !(VoterList & (1 << 4))) return 4;
	else if(strstr((char*)TagArray, USER5) != '\0' && !(VoterList & (1 << 5))) return 5;
	else if(strstr((char*)TagArray, USER6) != '\0' && !(VoterList & (1 << 6))) return 6;
	else if(strstr((char*)TagArray, USER7) != '\0' && !(VoterList & (1 << 7))) return 7;
	else if(strstr((char*)TagArray, USER8) != '\0' && !(VoterList & (1 << 8))) return 8;
	else if(strstr((char*)TagArray, USER9) != '\0' && !(VoterList & (1 << 9))) return 9;
	else if(strstr((char*)TagArray, USERA) != '\0' && !(VoterList & (1 << 10))) return 10;
	else if(strstr((char*)TagArray, USERX) != '\0') return 15;
	return False;
}	

void ProceedToVote(unsigned char ID)
{
	LCD_Disp(0x80,"  SELECT PARTY  ");
	LCD_Disp(0xC0,"S -> P1  I -> P2");
	
	do
	{
		LastDetectedKey = FetchKey(&KeyDebounceTime);
	}
	while(LastDetectedKey != SET && LastDetectedKey != INC);
	DelayMS(250); LCDClear();
	
	if(LastDetectedKey == SET)
	{
		LCD_Disp(0x80,"UR VOTE ENROLED ");
		Party1 += 1; TotalCounts++;
		VoterList |= (1 << ID);
	}
	else if(LastDetectedKey == INC)
	{
		LCD_Disp(0x80,"UR VOTE ENROLED ");
		Party2 += 1; TotalCounts++;
		VoterList |= (1 << ID);
	}
	else
	{
		LCD_Disp(0x80,"NOT A VALID KEY ");
		LCD_Disp(0xC0,"PLEASE TRY AGAIN");
	}	
	
	DelayMS(500); LCDClear();
}			

void main()
{
	TRIS_OUTPUT(Alarm);
	PORT_LOW(Alarm);
	
	Keypad_Initialize(KeySET, KeyMOV, KeyENT, KeyINC, KeyDEC);
	LCD_Initialize(RS, EN, D4, D5, D6, D7);
	RFID_Initialize(SS, SCK, MOSI, MISO, Interval);
	FP_Initialize(FPBaudRate, FPTouch);
	
	LCD_Disp(0x80, " SOLAR AND FPID ");
	LCD_Disp(0xC0, " VOTING SYSTEM  ");
	DelayMS(2500); LCDClear();
	
	if(ESP_Initialize(ESPBaudRate)) LCD_Disp(0x80,"ESP MODEL FOUND");
	else LCD_Disp(0x80,"ESP NOT FOUND"); DelayMS(500); LCDClear();
	
	if(ESP_Mode(3)) LCD_Disp(0x80,"MODE CONFIG");
	else LCD_Disp(0x80,"MODE ERROR"); DelayMS(500); LCDClear();
	
	if(ESP_WiFi()) LCD_Disp(0x80,"WIFI CONNECTED");
	else LCD_Disp(0x80,"WIFI ERROR"); DelayMS(500); LCDClear();
	
	if(ESP_Start(ServerName, ServerPort)) LCD_Disp(0x80,"SERVER CONNECT");
	else LCD_Disp(0x80,"SERVER ERROR"); DelayMS(500); LCDClear();
	
	if(MQTT_Connect()) LCD_Disp(0x80,"MQTT CONNECTED");
	else LCD_Disp(0x80,"MQTT ERROR"); DelayMS(500); LCDClear();
	
	if(MQTT_Publish(VariableLabel1, False)) LCD_Disp(0x80,"DATA PUBLISH");
	else LCD_Disp(0x80,"PUBLISH ERROR"); DelayMS(500); LCDClear();
	
	if(MQTT_Publish(VariableLabel2, False)) LCD_Disp(0x80,"DATA PUBLISH");
	else LCD_Disp(0x80,"PUBLISH ERROR"); DelayMS(500); LCDClear();
	
	if(MQTT_Publish(VariableLabel3, False)) LCD_Disp(0x80,"DATA PUBLISH");
	else LCD_Disp(0x80,"PUBLISH ERROR"); DelayMS(500); LCDClear();
	
	if(MQTT_Publish(VariableLabel4, False)) LCD_Disp(0x80,"DATA PUBLISH");
	else LCD_Disp(0x80,"PUBISH ERROR"); DelayMS(500); LCDClear();
	
	while(1)
	{
		LastDetectedKey = FetchKey(&KeyDebounceTime);
		CardDetected = ReadRFID();
		
		LCD_Disp(0x80," SHOW YOUR CARD ");
		
		if(CardDetected) ISValidCard = ReturnCard();
		
		if(CardDetected && !ISValidCard)
		{
			LCD_Disp(0x80, "   WARNING!!!  ");
			PORT_HIGH(Alarm); DelayMS(1000);
			PORT_LOW(Alarm); DelayMS(100);
			CardDetected = False; 
			ISValidCard = False;
		}
		
		if(CardDetected && ISValidCard)
		{
			if(ISValidCard == 15 && FP_Verify(ISValidCard, &MatchingScore))
			{
				LCD_Disp(0x80,"  FINAL RESULT  ");
				
				LCD_Disp(0xC0,"P1:");
				LCD_Decimal(0xC3, Party1, 2, DEC);
				
				LCD_Disp(0xC6,"P2:");
				LCD_Decimal(0xC9, Party2, 2, DEC);
				
				MQTT_Publish(VariableLabel2, Party1);
				MQTT_Publish(VariableLabel3, Party2);
				MQTT_Publish(VariableLabel4, (Party1 > Party2 ?True :2));
				while(1);
			}	
			else if(FP_Verify(ISValidCard, &MatchingScore)) 
			{
				ProceedToVote(ISValidCard);
				MQTT_Publish(VariableLabel1, TotalCounts);
			}
			else
			{
				PORT_HIGH(Alarm); DelayMS(500);
				LCD_Disp(0x80," PLS TRY AGAIN  ");
				PORT_LOW(Alarm); DelayMS(500);
			}	
		}	
		
		if(LastDetectedKey == SET && KeyDebounceTime > 300)
		{
			while(!ReadRFID()); ISValidCard = ReturnCard();
			if(FP_EnrollMent(ISValidCard)) LCD_Disp(0xC0,"FINGER ENROLLED");
			else LCD_Disp(0xC0,"FAIL TO ENROLL"); DelayMS(500); LCDClear();
		}
		if(LastDetectedKey == MOV && KeyDebounceTime > 300)
		{
			if(FP_Identify(&FingerID, &MatchingScore))
			{
				LCD_Disp(0xC0,"MATCHED");
				LCD_Decimal(0xC8, FingerID, 2, DEC);
				LCD_Decimal(0xCB, MatchingScore, 2, DEC);
			}
			else LCD_Disp(0xC0,"NOT MATCHED");
			DelayMS(500); LCDClear();
		}
		if(LastDetectedKey == INC && KeyDebounceTime > 300)
		{
			if(FP_DeleteAll()) LCD_Disp(0xC0,"FINGERS DELETED");
			else LCD_Disp(0xC0,"DELETING ERROR"); DelayMS(500); LCDClear();
		}	
	}
}				
				
		
			
			
				
				
				
				
			
		