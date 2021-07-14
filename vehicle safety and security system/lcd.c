#include <htc.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _GPIO_H
#include "gpio.h"
#endif

#include "lcd.h"

char RegisterSelect;
char PinEnable;
char PinD4;
char PinD5;
char PinD6;
char PinD7;

void LCD_Enable()
{
	PORT_LOW(PinEnable); __delay_us(5);
	PORT_HIGH(PinEnable); __delay_us(5);
	PORT_LOW(PinEnable); DelayUS(100);
}

void LCDClear()
{
	LCD_Command(CLEAR_DISPLAY);
	DelayMS(20);
}

void LCD_Initialize(char RS, char EN, char D4, char D5, char D6, char D7)
{
	RegisterSelect = RS;
	PinEnable = EN;
	PinD4 = D4;
	PinD5 = D5;
	PinD6 = D6;
	PinD7 = D7;
	
	TRIS_OUTPUT(RegisterSelect);
	TRIS_OUTPUT(PinEnable);
	TRIS_OUTPUT(PinD4);
	TRIS_OUTPUT(PinD5);
	TRIS_OUTPUT(PinD6);
	TRIS_OUTPUT(PinD7);
	DelayMS(250);
	
	LCD_Command(0x03); __delay_ms(5);
	LCD_Command(0x03); __delay_ms(5);
	LCD_Command(0x03); DelayUS(150);
	
	LCD_Command(RETURN_HOME); DelayUS(150);
	LCD_Command(FOUR_BIT | TWO_LINE | CHAR_SET1); DelayUS(150);
	LCD_Command(DISPLAY_ON | CURSOR_OFF | BLINK_OFF); DelayUS(150);
	LCD_Command(SHIFT_DISABLE | CUR_INCREMENT); DelayUS(150);
	LCDClear();
}


void LCD_Command(unsigned char CommandData)
{
	PORT_LOW(RegisterSelect);
	LCD_Allort(CommandData & 0xF0); LCD_Enable();
	LCD_Allort((CommandData << 4) & 0xF0); LCD_Enable();
}	

void LCD_Write(unsigned char CommandData, unsigned char DisplayData)
{
	LCD_Command(CommandData);
	PORT_HIGH(RegisterSelect);
	LCD_Allort(DisplayData & 0xF0); LCD_Enable();
	LCD_Allort((DisplayData << 4) & 0xF0); LCD_Enable();
}

void LCD_Disp(unsigned char DisplayAddress, const char *DisplayData)
{
	while(*DisplayData) LCD_Write(DisplayAddress++, *DisplayData++);
}

void LCD_CustomChar(unsigned char Location, const char *DisplayData)
{
	Location *= 8;
	for(char k = 0; k < 8; k++) LCD_Write((CGRAM_ADDRESS + Location + k), DisplayData[k]);
}

void LCD_CustomDisp(unsigned char DisplayAddress, unsigned char T_Location, const char *DisplayData)
{
	LCD_CustomChar(T_Location, DisplayData); 
	LCD_Write(DisplayAddress, T_Location);
}	

void LCD_Decimal(unsigned char DisplayAddress, unsigned long DecimalValue, int Length, int Base)
{
	register unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}

void LCD_Float(unsigned char DisplayAddress, float Value, int PostFix, int PreFix, int Seg)
{
	register unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}

void LCD_Allort(unsigned char Data)
{
	if(Data & 0x10) PORT_HIGH(PinD4); else PORT_LOW(PinD4);
	if(Data & 0x20) PORT_HIGH(PinD5); else PORT_LOW(PinD5);
	if(Data & 0x40) PORT_HIGH(PinD6); else PORT_LOW(PinD6);
	if(Data & 0x80) PORT_HIGH(PinD7); else PORT_LOW(PinD7);
}	
