#include <htc.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

__CONFIG(1, HSPLL);
__CONFIG(2, PWRTEN & BOREN & WDTDIS);
__CONFIG(3, MCLREN & PBDIGITAL & CCP2RC1);
__CONFIG(4, DEBUGDIS & LVPDIS & STVREN & XINSTDIS);

#include "gpio.h"
#include "utils.h"
#include "lcd.h"
#include "int.h"
#include "timer.h"
#include "uart.h"
#include "suart.h"
#include "sim800l.h"
#include "matrix.h"

#ifndef LCDPinConfig
#define LCDPinConfig
#define RS		 PB5
#define EN		 PB4
#define D4		 PB3
#define D5		 PB2
#define D6		 PB1
#define D7		 PB0
#endif

#ifndef MatrixConfig
#define MatrixConfig
#define KeypadROW1	PD0
#define KeypadROW2	PD1
#define KeypadROW3	PD2
#define KeypadROW4	PD3
#define KeypadCOL1	PD4
#define KeypadCOL2	PD5
#define KeypadCOL3	PD6
#define KeypadCOL4	0
#endif

#ifndef MotorConfig
#define MotorConfig
#define MotorRelay  PD7
#endif

#ifndef PanelConfig
#define PanelConfig
#define PanelKey1	PB6
#define PanelKey2	PB7
#endif

#ifndef TiltConfig
#define TiltConfig
#define TiltSensor  PC2
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define Alarm		PC3
#endif

#ifndef UserConfig
#define UserConfig
#define PanicInterval 10000
#define MasterPWD     "2205"
#define MISMATCH       1
#define ACCIDENT	   2
#define QUERIED		   3
#define LOCK		   4
#endif

extern unsigned char SSerialArray[100], SSerialCount;
unsigned char LocalArray[12], PassWord[8], LastDetectedKey;
unsigned char MobileNumber1[12], MobileNumber2[12];
unsigned char GPSCount1, GPSCount2, KeyCount;
unsigned int KeyDebounceTime;
float Lattitude, Longitude;

bit ISVehicleStarted, ISPassWordMatched, ISUserNormal;

void SSerialRoutine()
{
	SSerialArray[GPSCount1] = SSerial_Receive();
	if(SSerialArray[0] == '$') GPSCount1 = 1;
	if(SSerialArray[0] == '$' && SSerialArray[1] == 'G') GPSCount1 = 2;
	if(SSerialArray[0] == '$' && SSerialArray[1] == 'G' && SSerialArray[2] == 'P') GPSCount1 = 3;
	if(SSerialArray[0] == '$' && SSerialArray[1] == 'G' && SSerialArray[2] == 'P' && SSerialArray[3] == 'G') GPSCount1 = 4;
	if(SSerialArray[0] == '$' && SSerialArray[1] == 'G' && SSerialArray[2] == 'P' && SSerialArray[3] == 'G' && SSerialArray[4] == 'L') GPSCount1 = 5;
	if(SSerialArray[0] == '$' && SSerialArray[1] == 'G' && SSerialArray[2] == 'P' && SSerialArray[3] == 'G' && SSerialArray[4] == 'L' && SSerialArray[5] == 'L')
	GPSCount1 = GPSCount2++;
	CCP2IF = 0;
}		

unsigned char GPS_Update()
{
	register unsigned char LocalArray1[12], LocalArray2[12];
	float RefVariable = 0.0F;
	
	SSerial_Flush();
	GPSCount1 = 0; GPSCount2 = 6;
	SSerial_Enable(True);
	while(GPSCount1 < 50);
	SSerial_Enable(False);
	
	memset(LocalArray1, '\0', sizeof(LocalArray1));
	memset(LocalArray2, '\0', sizeof(LocalArray2));
	
	LocalArray2[0] = SSerialArray[7]; LocalArray2[1] = SSerialArray[8];
	for(unsigned char Variable = 9; SSerialArray[Variable] != ','; Variable++)
	LocalArray1[Variable - 9] = SSerialArray[Variable];
	RefVariable = atof((char*)LocalArray2);
	Lattitude = RefVariable + (atof((char*)LocalArray1) / 60);
	
	LocalArray2[0] = SSerialArray[20]; LocalArray2[1] = SSerialArray[21]; LocalArray2[2] = SSerialArray[22];
	for(unsigned char Variable = 23; SSerialArray[Variable] != ','; Variable++)
	LocalArray1[Variable - 23] = SSerialArray[Variable];
	RefVariable = atof((char*)LocalArray2);
	Longitude = RefVariable + (atof((char*)LocalArray1) / 60);
	
	return SSerialArray[44];
}

void SendMessage(const char *MobNumber, unsigned char Status)
{
	GPS_Update(); SSerial_Flush();
	unsigned char LocalArrayS[12];
	
	if(Status == MISMATCH) strcpy(SSerialArray, "PassWord Mismatched\n");
	else if(Status == ACCIDENT) strcpy(SSerialArray, "Accident Occured at\n");
	else if(Status == QUERIED) strcpy(SSerialArray, "Queried Location \n");
	else if(Status == LOCK) strcpy(SSerialArray, "Condition Executed\n");
	
	strcat(SSerialArray, "https://www.google.co.in/maps/search/");
	ftostra(Lattitude, 2, 4, LocalArrayS, NoSign);
	strncat(SSerialArray, LocalArrayS, strlen(LocalArrayS));
	
	strcat(SSerialArray, ",");
	ftostra(Longitude, 2, 4, LocalArrayS, NoSign);
	strncat(SSerialArray, LocalArrayS, strlen(LocalArrayS));
	
	LCD_Write(0x8F, 'M');
	GSM_SendMessage(MobNumber, SSerialArray);
	LCD_Write(0x8F, ' ');
	Serial_Flush(); SSerial_Flush();
}	

void main()
{
	TRIS_OUTPUT(MotorRelay);
	TRIS_OUTPUT(Alarm);
	TRIS_INPUT(TiltSensor);
	TRIS_INPUT(PanelKey1);
	TRIS_INPUT(PanelKey2);
	
	PORT_LOW(MotorRelay);
	PORT_LOW(Alarm);
	
	Matrix_Initialize(KeypadROW1, KeypadROW2, KeypadROW3, KeypadROW4,
					  KeypadCOL1, KeypadCOL2, KeypadCOL3, KeypadCOL4);
	LCD_Initialize(RS, EN, D4, D5, D6, D7);
	SSerial_Initialize(9600);
	EnableInterrupt(CP2INT, SSerialRoutine);
	
	LCD_Disp(0x80,"VEHICLE SAFETY &");
	LCD_Disp(0xC0,"SECURITY SYSTEM ");
	DelayMS(2500); LCDClear(); 
	
	for(unsigned char k = 0; k < 10; k++)
	{
		LCD_Disp(0x80,"+91"); LCD_Disp(0xC0,"+91");
		MobileNumber1[k] = eeprom_read(0x00 + k);
		LCD_Write(0x83 + k, eeprom_read(0x00 + k));
		MobileNumber2[k] = eeprom_read(0x10 + k);
		LCD_Write(0xC3 + k, eeprom_read(0x10 + k));
	}	
	DelayMS(1500); LCDClear();
	
	for(unsigned char k = 0; k < 4; k++)
	PassWord[k] = eeprom_read(0x20 + k);
	
	if(GSM_Initialize()) LCD_Disp(0x80,"GSM FOUND...");
	else LCD_Disp(0x80,"GSM NOT FOUND"); DelayMS(500); LCDClear();
	
	if(GSM_CheckSignal()) LCD_Disp(0x80,"SIM READY...");
	else LCD_Disp(0x80,"SIM ERROR"); DelayMS(500); LCDClear();
	
	Serial_Write("AT+CMGF=1");
	Serial_Flush();
	if(STalkBack(True, "OK", 2, 2500)) LCD_Disp(0x80,"MSG READY");
	else LCD_Disp(0x80,"MSG ERROR"); DelayMS(500); LCDClear();
	
	Serial_Write("AT+CNMI=2,2,0,0,0");
	Serial_Flush();
	if(STalkBack(True, "OK", 2, 2500)) LCD_Disp(0x80,"CALL READY");
	else LCD_Disp(0x80,"CALL ERROR"); DelayMS(500); LCDClear();
	
	Serial_Flush(); Timer_Initialize();
	
	while(1)
	{
		LastDetectedKey = FetchKey(&KeyDebounceTime);
		LCD_Float(0x80, Lattitude, 2, 4, NoSign);
		LCD_Float(0x88, Longitude, 2, 4, NoSign);
		
		if(isdigit(LastDetectedKey) && !ISPassWordMatched)
		{
			LocalArray[KeyCount] = LastDetectedKey;
			LCD_Write(0xC0 + KeyCount, LastDetectedKey);
			++KeyCount;
		}
		if(KeyCount > 3 && !ISPassWordMatched)
		{
			if(strcmp((char*)LocalArray, PassWord) == 0)
			{
				ISPassWordMatched = True;
				LCD_Disp(0xC0,"MATCH");
			}
			else
			{
				LCD_Disp(0xC0,"ERROR");
				PORT_HIGH(Alarm);
				SendMessage(MobileNumber1, MISMATCH);
				PORT_LOW(Alarm);
				ISPassWordMatched = False;
			}
			memset(LocalArray, '\0', sizeof(LocalArray));
			DelayMS(500); LCDClear(); KeyCount = 0;
		}
		
		if(ISPassWordMatched && !PORT_READ(PanelKey2) && !ISVehicleStarted)
		{
			PORT_LOW(Alarm);
			PORT_HIGH(MotorRelay);
			ISVehicleStarted = True;
		}
		if(ISVehicleStarted && PORT_READ(PanelKey2))
		{
			PORT_LOW(MotorRelay);
			ISVehicleStarted = False;
			ISPassWordMatched = False;
		}	
		
		if(ISVehicleStarted && GSM_Incoming(LocalArray, DECLINE))
		{
			LCD_Disp(0xC0, (char*)LocalArray);
			SendMessage(LocalArray, QUERIED); 
			LCD_Disp(0xC0,"                ");
			memset(LocalArray, '\0', sizeof(LocalArray));
		}
		
		if(ISVehicleStarted && GSM_ReceiveMessage(LocalArray))
		{
			LCD_Disp(0xC0,"STATUS RECEIVED ");
			if(strcmp(LocalArray, MobileNumber1) == 0)
			{
				if(strstr((char*)SerialArray, "OFF") != '\0')
				{
					SendMessage(MobileNumber1, LOCK);
					PORT_LOW(MotorRelay);
				}
				if(strstr((char*)SerialArray, "ON") != '\0')
				{
					SendMessage(MobileNumber2, LOCK);
					PORT_HIGH(MotorRelay);
				}	
			}
			DelayMS(500); LCDClear();
			memset(LocalArray, '\0', strlen(LocalArray));
		}
		if(ISVehicleStarted && !PORT_READ(TiltSensor))
		{
			LCD_Disp(0xC0,"ACCIDENT OCCURED");
			PORT_HIGH(Alarm); PORT_LOW(MotorRelay);
			unsigned long PanicTimer = Millis() + PanicInterval;
			
			do
			{
				if(!PORT_READ(PanelKey1))
				{
					ISUserNormal = True;
					break;
				}
				else ISUserNormal = False;
			}
			while(Millis() < PanicTimer);
			
			if(!ISUserNormal)
			{
				LCD_Disp(0xC0, "  UNCONSCIOUS   ");
				SendMessage(MobileNumber2, ACCIDENT);
				ISVehicleStarted = False;
				ISPassWordMatched = False;
			}
			else
			{
				LCD_Disp(0xC0,"CONSCIOUS STATE ");
				PORT_LOW(Alarm); 
			}
			DelayMS(500); LCDClear();
		}
		
		if(LastDetectedKey == '*' && KeyDebounceTime < 300)
		{
			LCD_Disp(0x80,"MOB1:"); 
			LCD_Disp(0x85, MobileNumber1);
			LCD_Disp(0xC0,"MOB2:");
			LCD_Disp(0xC5, MobileNumber2);
			DelayMS(1500); LCDClear();
		}
		if(LastDetectedKey == '*' && KeyDebounceTime > 300)
		{
			LCDClear(); LCD_Disp(0x80," ENTER MOB NUM1 ");
			memset(LocalArray, '\0', strlen(LocalArray));
			Matrix(0xC0, LocalArray, 10);
			for(unsigned int k = 0; k < 10; k++)
			{
				MobileNumber1[k] = LocalArray[k];
				eeprom_write(0x00 + k, MobileNumber1[k]);
			}	
			DelayMS(500); LCDClear();
			
			LCD_Disp(0x80," ENTER MOB NUM2 ");
			memset(LocalArray, '\0', strlen(LocalArray));
			Matrix(0xC0, LocalArray, 10); 
			for(unsigned int k = 0; k < 10; k++)
			{
				MobileNumber2[k] = LocalArray[k];
				eeprom_write(0x10 + k, MobileNumber2[k]);
			}		
			DelayMS(500); LCDClear();
			memset(LocalArray, '\0', strlen(LocalArray));
		}
		
		if(LastDetectedKey == '#' && KeyDebounceTime < 300) GPS_Update();
		
		if(LastDetectedKey == '#' && KeyDebounceTime > 300)
		{
			LCDClear(); LCD_Disp(0x80," ENTER OLD PASS ");
			memset(LocalArray, '\0', sizeof(LocalArray));
			Matrix(0xC0, LocalArray, 4);
			
			if(strcmp((char*)LocalArray, PassWord) == 0 || strcmp((char*)LocalArray, MasterPWD) == 0)
			{
				LCDClear(); LCD_Disp(0x80," ENTER NEW PASS ");
				memset(LocalArray, '\0', sizeof(LocalArray));
				Matrix(0xC0, LocalArray, 4);
				
				for(unsigned char k = 0; k < 4; k++)
				{
					eeprom_write(0x20 + k, LocalArray[k]);
					PassWord[k] = LocalArray[k];
				}
				
				LCD_Disp(0xC0,"PASSWORD CHANGED");
			}
			else
			{
				LCD_Disp(0x80," WRONG OLD PASS");
				PORT_HIGH(Alarm);
			}
			
			memset(LocalArray, '\0', strlen(LocalArray));
			DelayMS(500); LCDClear();
			PORT_LOW(Alarm);
		}
	}
}				
				
				
			
						
				
			
							
					
						
					
					
		
		