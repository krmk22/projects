#include <htc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef _GPIO_H
#include "gpio.h"
#endif

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _LCD_H
#include "lcd.h"
#endif

#include "matrix.h"

char KeypadRow1;
char KeypadRow2;
char KeypadRow3;
char KeypadRow4;

char KeypadCol1;
char KeypadCol2;
char KeypadCol3;
char KeypadCol4;

void Matrix_Initialize(char Row1, char Row2, char Row3, char Row4,
					   char Col1, char Col2, char Col3, char Col4)
{
	KeypadRow1 = Row1;
	KeypadRow2 = Row2;
	KeypadRow3 = Row3;
	KeypadRow4 = Row4;
	
	KeypadCol1 = Col1;
	KeypadCol2 = Col2;
	KeypadCol3 = Col3;
	KeypadCol4 = Col4;
	
	TRIS_OUTPUT(KeypadRow1);
	TRIS_OUTPUT(KeypadRow2);
	TRIS_OUTPUT(KeypadRow3);
	TRIS_OUTPUT(KeypadRow4);
	
	TRIS_INPUT(KeypadCol1);
	TRIS_INPUT(KeypadCol2);
	TRIS_INPUT(KeypadCol3);
	if(KeypadCol1) TRIS_INPUT(KeypadCol4);
}

unsigned char FetchKey(unsigned int *Var1)
{
	unsigned char DataArray[] = "123A456B789C*0#D";
	unsigned int ButtonPressedTime = 0x00;
	unsigned char RowArray[5];
	
	RowArray[0] = KeypadRow1;
	RowArray[1] = KeypadRow2;
	RowArray[2] = KeypadRow3;
	RowArray[3] = KeypadRow4;
	
	for(unsigned char k = 0; k < 5; k++)
	{
		while(!PORT_READ(KeypadCol1));
		while(!PORT_READ(KeypadCol2));
		while(!PORT_READ(KeypadCol3));
		if(KeypadCol4) while(!PORT_READ(KeypadCol4));
		
		for(unsigned char j = 0; j < 5; j++)
		{
			if(j == k) PORT_LOW(RowArray[j]);
			else PORT_HIGH(RowArray[j]);
		}	
		
		if(!PORT_READ(KeypadCol1))
		{
			while(!PORT_READ(KeypadCol1))
			{
				*Var1 = ++ButtonPressedTime;
				__delay_ms(1);
			}
			return DataArray[(k * 4) + 0];
		}
		if(!PORT_READ(KeypadCol2))
		{
			while(!PORT_READ(KeypadCol2))
			{
				*Var1 = ++ButtonPressedTime;
				__delay_ms(1);
			}
			return DataArray[(k * 4) + 1];
		}				
		if(!PORT_READ(KeypadCol3))
		{
			while(!PORT_READ(KeypadCol3))
			{
				*Var1 = ++ButtonPressedTime;
				__delay_ms(1);
			}
			return DataArray[(k * 4) + 2];
		}	
		if(KeypadCol4 && !PORT_READ(KeypadCol4))
		{
			while(!PORT_READ(KeypadCol4))
			{
				*Var1 = ++ButtonPressedTime;
				__delay_ms(1);
			}
			return DataArray[(k * 4) + 3];
		}
	}
	return '\0';
}		

void Matrix(unsigned char ReqPos, unsigned char *ReqArr, unsigned char ReqDig)
{
	unsigned char Variable = 0;
	unsigned char MaximumMove = 0;
	unsigned int KeyFetchTime;
	LCD_Command(CURSOR_ON);
	
	do
	{
		Variable = FetchKey(&KeyFetchTime);
		
		if(isdigit(Variable))
		{
			ReqArr[MaximumMove] = Variable;
			LCD_Write(ReqPos + MaximumMove, Variable);
			if(++MaximumMove > (ReqDig - 1)) MaximumMove = 0;
		}
		if(Variable == '*' && KeyFetchTime < 300)
		{
			if(--MaximumMove > (ReqDig - 1))
			MaximumMove = (ReqDig - 1);
			LCD_Write(ReqPos + MaximumMove, ' ');
		}
		if(Variable == '*' && KeyFetchTime > 300)
		{
			for(unsigned char k = 0; k < ReqDig; k++)
			LCD_Write(ReqPos + k, ' ');
			memset(ReqArr, '\0', sizeof(ReqArr));
			MaximumMove = 0;
		}
	}
	while(Variable != '#');
	LCD_Command(CURSOR_OFF);
}

void RTC_Keypad(unsigned char *StoreArray)
{	
	unsigned char LocalArray[4];
	
	LCDClear(); 
	LCD_Disp(0x80,"TIME:  -  -     ");
	LCD_Disp(0xC0,"DATE:  /  /     ");
	LCD_Command(CURSOR_ON);
	
	Matrix(0x85, LocalArray, 2);
	StoreArray[0] = atoi(LocalArray);
	Matrix(0x88, LocalArray, 2);
	StoreArray[1] = atoi(LocalArray);
	Matrix(0x8B, LocalArray, 2);
	StoreArray[2] = atoi(LocalArray);
	Matrix(0xC5, LocalArray, 2);
	StoreArray[3] = atoi(LocalArray);
	Matrix(0xC8, LocalArray, 2);
	StoreArray[4] = atoi(LocalArray);
	Matrix(0xCB, LocalArray, 2);
	StoreArray[5] = atoi(LocalArray);
	LCD_Command(CURSOR_OFF);
}