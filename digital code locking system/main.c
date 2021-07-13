#include <reg52.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "lcd.h"
#include "servo.h"

#ifndef MatrixKeyPadConfig
#define MatrixKeyPadConfig
sbit Col1 = P0^0;
sbit Col2 = P0^1;
sbit Col3 = P0^2;
sbit Row1 = P0^3;
sbit Row2 = P0^4;
sbit Row3 = P0^5;
sbit Row4 = P0^6;
#endif

#ifndef AlarmConfig
#define AlarmConfig
#define ActivateAlarm() P2 |= (1 << 3)
#define DeActivateAlarm() P2 &=~ (1 << 3)
#endif

#ifndef SecureConfig
#define SecureConfig
#define MainPassWord 5457
#define MasterPassWord 1111
#endif

unsigned char MatrixKeyPad();
unsigned char ServoPosition;

unsigned char GlobalArray[5];
unsigned char FetchKey = 0, KeyCount = 0;
unsigned int GlobalVariable = 0;
char ISDoorOpened = 0;

void main()
{
	DeActivateAlarm();
	LCD_Initialize();
	Servo_Initialize();
	LCD_Disp(0x80,"**DIGITAL CODE**");
	LCD_Disp(0xC0,"*LOCKING SYSTEM*");
	DelayMS(1000); LCDClear();
	ServoPosition = 180;
	memset((char*)GlobalArray, '\0', sizeof(GlobalArray));
	
	while(1)
	{
		LCD_Disp(0x80,"*ENTER PASSWORD*");
		FetchKey = MatrixKeyPad();

		if(KeyCount == 0 && (FetchKey == '#' || FetchKey == '*') && ISDoorOpened)
		{
			ISDoorOpened = 0;
			ServoPosition = 180;
			KeyCount = 0;
		}
		else if(isdigit(FetchKey))
		{			
			GlobalArray[KeyCount] = FetchKey;
			LCD_Write(0xC6 + KeyCount, GlobalArray[KeyCount]);
			DelayMS(50); DelayMS(50); DelayMS(50); 
			LCD_Write(0xC6 + KeyCount, '*');
			KeyCount++;
		}
		
		if(KeyCount > 3)
		{
			GlobalVariable = atoi((char*)GlobalArray);
			
			if(GlobalVariable == MainPassWord || GlobalVariable == MasterPassWord)
			{
				LCD_Disp(0x80, "PASSWORD MATCHED");
				ServoPosition = 90;
				ISDoorOpened = 1; DelayMS(1000);
				LCD_Disp(0xC0,"                 ");
			}
			else
			{
				LCD_Disp(0x80, "PASWORD NT MATCH");
				ActivateAlarm(); ServoPosition = 180;
				ISDoorOpened = 0; DelayMS(2500);
				LCD_Disp(0xC0,"                 ");
				DeActivateAlarm();
			}
			memset((char*)GlobalArray, '\0', sizeof(GlobalArray));
			KeyCount = 0;
		}
	}
}

unsigned char MatrixKeyPad()
{
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 0; Col2 = 1; Col3 = 1;
	if(!Row1){while(!Row1); return '1';}
	if(!Row2){while(!Row2); return '4';}
	if(!Row3){while(!Row3); return '7';}
	if(!Row4){while(!Row4); return '*';}
	
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 1; Col2 = 0; Col3 = 1;
	if(!Row1){while(!Row1); return '2';}
	if(!Row2){while(!Row2); return '5';}
	if(!Row3){while(!Row3); return '8';}
	if(!Row4){while(!Row4); return '0';}
	
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 1; Col2 = 1; Col3 = 0;
	if(!Row1){while(!Row1); return '3';}
	if(!Row2){while(!Row2); return '6';}
	if(!Row3){while(!Row3); return '9';}
	if(!Row4){while(!Row4); return '#';}
	
	return '\0';
}