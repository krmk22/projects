#include <pic.h>
#include <string.h>
#include <stdlib.h>

#include "clock.h"
#include "header.h"
#include "lcd.h"
#include "int.h"
#include "uart.h"
#include "keypad.h"

#ifndef FPSensorPinConfig
#define FPSensorPinConfig
#define Red "VCC"
#define Black "GND"
#define Yellow "TXD"
#define White "RXD"
#define GREEN "TOUCH"
#define BLUE "3.3V"
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define AlarmPort PORTD
#define AlarmDirection TRISD 
#define AlarmPin 6
#endif

#ifndef SolenoidConfig
#define SolenoidConfig
#define SolenoidPort PORTD
#define SolenoidDirection TRISD 
#define SolenoidPin 7
#endif

char CollectFingerImage();
char ConvertImage2Char(unsigned char);
char FingerEnrollMent(unsigned char);
char FingerIdentify(unsigned char*, unsigned char*);
char FingerVerify(unsigned char, unsigned char*);
char FingerDelete(unsigned char);
char FingerDeleteAll();
char FingerTalkBack(const char*, unsigned int, unsigned int);

unsigned char SerialArray[20], SerialCount;
unsigned char LastDetectedKey, MatchingScore, FingerID;
unsigned char StoreIDArray[5], StoreID; 
volatile unsigned int KeyDebounceTime;

void SerialRoutine()
{
	SerialArray[SerialCount] = RCREG;
	SerialCount = (SerialCount < 20 ?SerialCount + 1 :0);
	RCIF = 0;
}

void main()
{
	AlarmDirection &=~ (1 << AlarmPin);
	AlarmPort &=~ (1 << AlarmPin);
	SolenoidDirection &=~ (1 << SolenoidPin);
	SolenoidPort &=~ (1 << SolenoidPin);	
	
	Keypad_Initialize();
	LCD_Initialize();
	LCD_Disp(0x80,"FP BASED VEHICLE");
	LCD_Disp(0xC0,"ACCESSING SYSTEM");
	DelayMS(2500); LCDClear();
	
	Serial_Initialize();
	Serial_Enable(True);
	EnableInterrupt(RXRINT, SerialRoutine);
	GIE = PEIE = True;
	
	while(1)
	{
		LCD_Disp(0x80,"PLACE UR FINGER ");
		LCD_Disp(0xC0,"ON SENSOR MODULE");
		LastDetectedKey = FetchKey(&KeyDebounceTime);
		
		if(LastDetectedKey == SET)
		{
			LCDClear();
			memset(StoreIDArray, '\0', sizeof(StoreIDArray));
			LCD_Disp(0x80,"FINGER ENROLMENT");
			LCD_Disp(0xC0,"FINGER ID ->");
			Keypad(0xCD, (char*)StoreIDArray, 2);
			StoreID = atoi((char*)StoreIDArray);
			
			if(FingerDelete(StoreID)) LCD_Disp(0xC0,"FINGER DELETED");
			else LCD_Disp(0xC0,"FAIL 2 DELETE"); DelayMS(1000);
			for(char k = 16; k--;) LCD_Write(0xC0 + k, ' ');
			
			if(FingerEnrollMent(StoreID))
			{
				for(char k = 16; k--;) LCD_Write(0xC0 + k, ' ');
				LCD_Disp(0xC0,"SUCCESS");
			}
			else
			{
				for(char k = 16; k--;) LCD_Write(0xC0 + k, ' ');
				LCD_Disp(0xC0,"ENROLLMENT FAIL ");
			}
			DelayMS(1000); LCDClear();
		}			
		
		if(LastDetectedKey == MOV)
		{
			LCDClear();
			memset(StoreIDArray, '\0', sizeof(StoreIDArray));
			LCD_Disp(0x80," FINGER VERIFY  ");
			LCD_Disp(0xC0,"FINGER ID ->");
			Keypad(0xCD, (char*)StoreIDArray, 2);
			StoreID = atoi((char*)StoreIDArray);
			
			if(FingerVerify(StoreID, &MatchingScore))
			{
				LCD_Disp(0xC0,"FINGER MATCH");
				LCD_Decimal(0xCD, MatchingScore, 2, DEC);
			}
			else LCD_Disp(0xC0,"FINGER NOT MATCH");
			DelayMS(1000); LCDClear();
		}	
		
		if(LastDetectedKey == INC || LastDetectedKey == DIC)
		{
			LCDClear();
			LCD_Disp(0x80,"FINGER IDENTIFY");
			if(FingerIdentify(&FingerID, &MatchingScore))
			{
				LCD_Disp(0xC0,"                ");
				LCD_Disp(0xC0,"MATCHED");
				LCD_Decimal(0xC8, FingerID, 2, DEC);
				LCD_Decimal(0xCB, MatchingScore, 2, DEC);
				SolenoidPort |= (1 << SolenoidPin);
			}
			else
			{
				LCD_Disp(0xC0,"                ");
				LCD_Disp(0xC0,"FAIL 2 MATCH ");	
				AlarmPort |= (1 << AlarmPin);
			}
			DelayMS(1000); LCDClear();
			AlarmPort &=~ (1 << AlarmPin);
		}
		if(LastDetectedKey == ENT && KeyDebounceTime > 500)
		SolenoidPort &=~ (1 << SolenoidPin);
	}
}

char FingerTalkBack(const char *FeedData, unsigned int FDLength, unsigned int TimeDelay)
{
	do
	{
		if(smemmem((char*)SerialArray, SerialCount, FeedData, FDLength) != '\0') return True;
		if(SerialCount > 10 && SerialArray[9] != 0x00) return False;
		__delay_ms(1);
	}
	while(TimeDelay --> 0);
	return False;
}

char FingerDelete(unsigned char FingerID)
{
  unsigned char CheckSum = FingerID + 21;
  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x07\x0C", 10);
  Serial_Send(0x00); Serial_Send(FingerID); Serial_Writeln("\x00\x01\x00",3); Serial_Send(CheckSum);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500));
}

char FingerDeleteAll()
{
  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x0D\x00\x11", 12);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500));
}

char CollectFingerImage()
{
  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x01\x00\x05", 12);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 5000));
}

char ConvertImage2Char(unsigned char PageID)
{
  unsigned int CheckSum = PageID + 7;
  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x04\x02", 10);
  Serial_Send(PageID); Serial_Send(0x00); Serial_Send(CheckSum);
  return(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\07\x00\x03\x00", 10, 2500));
}

char FingerEnrollMent(unsigned char FingerID)
{
  unsigned char CheckSum = 15 + FingerID;
  
  if(CollectFingerImage())
  LCD_Disp(0xC0,"FINGER CAPTURED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");      

  if(ConvertImage2Char(1))
  LCD_Disp(0xC0,"FINGER STORED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  if(CollectFingerImage())
  LCD_Disp(0xC0,"FINGER CAPTURED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  if(ConvertImage2Char(2))
  LCD_Disp(0xC0,"FINGER STORED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\05\x00\x09", 12);
  if(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
  LCD_Disp(0xC0,"FINGER COMBINED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x06\x02", 11);
  Serial_Send(0x00); Serial_Send(FingerID); Serial_Send(0x00); Serial_Send(CheckSum);
  if(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
  LCD_Disp(0xC0,"FINGER STORED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");
  return True;
}

char FingerIdentify(unsigned char *FingerID, unsigned char *MatchScore)
{
  if(CollectFingerImage())
  LCD_Disp(0xC0,"FINGER CAPTURED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  if(ConvertImage2Char(1))
  LCD_Disp(0xC0,"FINGER STORED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x08\x1B", 10);
  Serial_Writeln("\x01\x00\x00\x01\x01\x00\x27", 7);
  if(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x07\x00", 10, 2500))
  {
    DelayMS(10);
    *MatchScore = SerialArray[13];
    *FingerID = SerialArray[11];
    LCD_Disp(0xC0,"FINGER MATCHED");
  }
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");
  return True;
}

char FingerVerify(unsigned char FingerID, unsigned char *MatchScore)
{
  unsigned char CheckSum = FingerID + 16;

  if(CollectFingerImage())
  LCD_Disp(0xC0,"FINGER CAPTURED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  if(ConvertImage2Char(1))
  LCD_Disp(0xC0,"FINGER CONVERTED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x06\x07\x02", 11);
  Serial_Send(0x00); Serial_Send(FingerID); Serial_Send(0x00); Serial_Send(CheckSum);
  if(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x03\x00", 10, 2500))
  LCD_Disp(0xC0,"FINGER READED");
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");

  memset(SerialArray, '\0', sizeof(SerialArray)); SerialCount = 0;
  Serial_Writeln("\xEF\x01\xFF\xFF\xFF\xFF\x01\x00\x03\x03\x00\x07", 12);
  if(FingerTalkBack("\xEF\x01\xFF\xFF\xFF\xFF\x07\x00\x05\x00", 10, 2500))
  {
    DelayMS(10);
    *MatchScore = SerialArray[11];
    LCD_Disp(0xC0,"FINGER MATCHED");
  }
  else return False;
  DelayMS(500);
  LCD_Disp(0xC0,"                ");
  return True;
}