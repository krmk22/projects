#include <pic.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif

#include "keypad.h"

void Keypad_Initialize()
{
	#if MC2KEY == FIVE_KEYPAD
	FiveKeypadDirection |= (1 << FiveKeypadKey1);
	FiveKeypadDirection |= (1 << FiveKeypadKey2);
	FiveKeypadDirection |= (1 << FiveKeypadKey3);
	FiveKeypadDirection |= (1 << FiveKeypadKey4);
	FiveKeypadDirection |= (1 << FiveKeypadKey5);
	#endif
	
	#if MC2KEY == MATRIX_KEYPAD
	MatrixColDirection &=~ (1 << MatrixColPin1);
	MatrixColDirection &=~ (1 << MatrixColPin2);
	MatrixColDirection &=~ (1 << MatrixColPin3);
	MatrixColDirection &=~ (1 << MatrixColPin4);
	MatrixRowDirection |= (1 << MatrixRowPin1);
	MatrixRowDirection |= (1 << MatrixRowPin2);
	MatrixRowDirection |= (1 << MatrixRowPin3);
	MatrixRowDirection |= (1 << MatrixRowPin4);
	#endif
}

char FetchKey(volatile unsigned int *Var1)
{
	unsigned int ButtonPressedTime = 0x00;
	
	#if MC2KEY == FIVE_KEYPAD
	if(!SetKey)
	{
		while(!SetKey)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return SET;
	}
	if(!MovKey)
	{
		while(!MovKey)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return MOV;
	}
	if(!IncKey)
	{
		while(!IncKey)
		*Var1 = ++ButtonPressedTime;
		return INC;
	}
	if(!DecKey)
	{
		while(!DecKey)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return DIC;
	}
	if(!EntKey)
	{
		while(!EntKey)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return ENT;
	}
	return '\0';
	#endif
	
	#ifndef MC2KEY == MATRIX_KEYPAD
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 0; Col2 = 1; Col3 = 1; Col4 = 1;
	if(!Row1)
	{
		while(!Row1)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '1';
	}
	if(!Row2)
	{
		while(!Row2)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '4';
	}
	if(!Row3)
	{
		while(!Row3)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '7';
	}
	if(!Row4)
	{
		while(!Row4)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '*';
	}
	
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 1; Col2 = 0; Col3 = 1; Col4 = 1;
	if(!Row1)
	{
		while(!Row1)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '2';
	}
	if(!Row2)
	{
		while(!Row2)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '5';
	}
	if(!Row3)
	{
		while(!Row3)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '8';
	}
	if(!Row4)
	{
		while(!Row4)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '0';
	}
	
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 1; Col2 = 1; Col3 = 0; Col4 = 1;
	if(!Row1)
	{
		while(!Row1)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '3';
	}
	if(!Row2)
	{
		while(!Row2)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '6';
	}
	if(!Row3)
	{
		while(!Row3)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '9';
	}
	if(!Row4)
	{
		while(!Row4)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return '#';
	}
	
	while(!Row1 && !Row2 && !Row3 && !Row4);
	Col1 = 1; Col2 = 1; Col3 = 1; Col4 = 0;
	if(!Row1)
	{
		while(!Row1)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return 'A';
	}
	if(!Row2)
	{
		while(!Row2)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return 'B';
	}
	if(!Row3)
	{
		while(!Row3)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return 'C';
	}
	if(!Row4)
	{
		while(!Row4)
		{
			*Var1 = ++ButtonPressedTime;
			__delay_ms(1);
		}	
		return 'D';	
	}
	return '\0';
	#endif
}

void Keypad(unsigned char RequirePosition, unsigned char *RequireArray, unsigned int RequireDigit)
{
	unsigned char Variable;
	unsigned char MaximumMove = 0, InnerCount = 0;
	unsigned int KeyFetchTime;
	LCD_Command(0x0E);
	
	#if MC2KEY == FIVE_KEYPAD
	do
	{
		Variable = FetchKey(&KeyFetchTime);
		
		if(Variable == SET && KeyFetchTime < 300)
		{
			LCD_Write(RequirePosition + MaximumMove, ' ');
			if(--MaximumMove > (RequireDigit - 1))
			MaximumMove = (RequireDigit - 1);
		}
		if(Variable == SET && KeyFetchTime > 300)
		{
			for(char k = 0; k < RequireDigit; k++)
			LCD_Write(RequirePosition, ' ');
			memset(RequireArray, '\0', sizeof(RequireArray));
			MaximumMove = (RequireDigit - 1);
		}
		if(Variable == MOV)
		{
			if(++MaximumMove > (RequireDigit - 1))
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
		
		RequireArray[MaximumMove] = InnerCount + '0';
		LCD_Write(RequirePosition + MaximumMove, RequireArray[MaximumMove]);
	}
	while(Variable != ENT);
	LCD_Command(0x0C);
	#endif
	
	#if MC2KEY == MATRIX_KEYPAD
	do
	{
		Variable = FetchKey(&KeyFetchTime);
		
		if(isdigit(Variable))
		{
			RequireArray[MaximumMove] = Variable;
			LCD_Write(RequirePosition + MaximumMove, Variable);
			if(++MaximumMove > (RequireDigit - 1))
			MaximumMove = 0;
		}
		if(Variable == '*' && KeyFetchTime < 300)
		{
			if(--MaximumMove > (RequireDigit - 1))
			MaximumMove = (RequireDigit - 1);
			LCD_Write(RequirePosition + MaximumMove, ' ');
		}
		if(Variable == '*' && KeyFetchTime > 300)
		{
			for(char k = 0; k < RequireDigit; k++)
			LCD_Write(RequirePosition, ' ');
			memset(RequireArray, '\0', sizeof(RequireArray));
			MaximumMove = (RequireDigit - 1);
		}
	}
	while(Variable != '#');
	LCD_Command(0x0C);
	#endif
}

void RTC_Keypad(unsigned char *StoreArray)
{	
	unsigned char LocalArray[4];
	
	LCDClear(); 
	LCD_Disp(0x80,"TIME:  -  -     ");
	LCD_Disp(0xC0,"DATE:  /  /     ");
	LCD_Command(0x0E);
	
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
}	
	