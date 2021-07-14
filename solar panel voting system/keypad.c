#include <htc.h>
#include <string.h>
#include <stdlib.h>

#ifndef _GPIO_H
#include "gpio.h"
#endif

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif

#include "keypad.h"

char KeypadSetKey;
char KeypadMovKey;
char KeypadIncKey;
char KeypadDecKey;
char KeypadEntKey;

void Keypad_Initialize(char Key1, char Key2, char Key3, char Key4, char Key5)
{
	KeypadSetKey = Key1;
	KeypadMovKey = Key2;
	KeypadEntKey = Key3;
	KeypadIncKey = Key4;
	KeypadDecKey = Key5;
	
	TRIS_INPUT(KeypadSetKey);
	TRIS_INPUT(KeypadMovKey);
	TRIS_INPUT(KeypadEntKey);
	TRIS_INPUT(KeypadIncKey);
	TRIS_INPUT(KeypadDecKey);
}	

unsigned char FetchKey(unsigned int *Var1)
{
	register unsigned int ButtonPressedTime = 0;
	
	if(!PORT_READ(KeypadSetKey))
	{
		while(!PORT_READ(KeypadSetKey))
		{
			*Var1 = ++ButtonPressedTime;
			DelayMS(1);
		}
		return SET;
	}
	if(!PORT_READ(KeypadMovKey))
	{
		while(!PORT_READ(KeypadMovKey))
		{
			*Var1 = ++ButtonPressedTime;
			DelayMS(1);
		}
		return MOV;
	}
	if(!PORT_READ(KeypadEntKey))
	{
		while(!PORT_READ(KeypadEntKey))
		{
			*Var1 = ++ButtonPressedTime;
			DelayMS(1);
		}
		return ENT;
	}
	if(!PORT_READ(KeypadIncKey))
	{
		while(!PORT_READ(KeypadIncKey))
		{
			*Var1 = ++ButtonPressedTime;
			DelayMS(1);
		}
		return INC;
	}
	if(!PORT_READ(KeypadDecKey))
	{
		while(!PORT_READ(KeypadDecKey))
		{
			*Var1 = ++ButtonPressedTime;
			DelayMS(1);
		}
		return DIC;
	}
	return '\0';
}									

void Keypad(unsigned char ReqPos, unsigned char *ReqArr, unsigned char ReqDig)
{
  register unsigned char Variable;
  register unsigned char MaximumMove;
  register unsigned char InnerCount;
  unsigned int KeyFetchTime;
  LCD_Command(CURSOR_ON);
  
  do
  {
    Variable = FetchKey(&KeyFetchTime);
    
    if(Variable == SET && KeyFetchTime < 300)
    {
      LCD_Write(ReqPos + MaximumMove, ' ');
      if(--MaximumMove > (ReqDig - 1))
      MaximumMove = (ReqDig - 1);
    }
    if(Variable == SET && KeyFetchTime > 300)
    {
      for(register char k = 0; k < ReqDig; k++)
      LCD_Write(ReqPos + k, ' ');
      memset(ReqArr, '\0', sizeof(ReqArr));
      MaximumMove = 0;
    }
    if(Variable == MOV)
    {
      if(++MaximumMove > (ReqDig - 1))
      MaximumMove = 0;
    }
    if(Variable == INC)
    {
      if(++InnerCount > 9)
      InnerCount = 0;
    }
    if(Variable == DIC)
    {
      if(--InnerCount > 9)
      InnerCount = 9;
    }
    
    ReqArr[MaximumMove] = InnerCount + '0';
    LCD_Write(ReqPos + MaximumMove, ReqArr[MaximumMove]);
  }
  while(Variable != ENT);
  LCD_Command(CURSOR_OFF);        
}

void RTC_Keypad(unsigned char *StoreArray)
{  
  unsigned char LocalArray[4];
  
  LCDClear(); 
  LCD_Disp(0x80,"TIME:  -  -     ");
  LCD_Disp(0xC0,"DATE:  /  /     ");
  LCD_Command(CURSOR_ON);
  
  Keypad(0x85, LocalArray, 2);
  StoreArray[0] = atoi(LocalArray);
  Keypad(0x88, LocalArray, 2);
  StoreArray[1] = atoi(LocalArray);
  Keypad(0x8B, LocalArray, 2);
  StoreArray[2] = atoi(LocalArray);
  Keypad(0xC5, LocalArray, 2);
  StoreArray[3] = atoi(LocalArray);
  Keypad(0xC8, LocalArray, 2);
  StoreArray[4] = atoi(LocalArray);
  Keypad(0xCB, LocalArray, 2);
  StoreArray[5] = atoi(LocalArray);
  LCD_Command(CURSOR_OFF);
} 