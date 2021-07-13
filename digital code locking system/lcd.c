#include <at89x52.h>
#include <intrins.h>

#ifndef _HEADER_H
#include "header.h"
#endif

#include "lcd.h"

void LCD_Initialize()
{
	DelayMS(100);
	LCD_Command(0x03);
	DelayMS(5);
	LCD_Command(0x03);
	DelayMS(5);
	LCD_Command(0x03);
	DelayUS(150);
	LCD_Command(0x02);
	DelayUS(150);
	LCD_Command(0x28);
	DelayMS(5);
	LCD_Command(0x28);
	DelayMS(5);
	LCD_Command(0x28);
	DelayUS(150);
	LCD_Command(0x06);
	DelayUS(150);
	LCD_Command(0x0C);
	DelayUS(150);
	LCDClear();
}

void LCD_Command(unsigned char CommandData)
{
	LCDControlPort &=~ (1 << RegisterSelect);
	LCD_Allort(CommandData & 0xF0); RollEnable();
	LCD_Allort((CommandData << 4) & 0xF0); RollEnable();
}

void LCD_Write(unsigned char CommandData, unsigned char DisplayData)
{
	LCD_Command(CommandData);
	LCDControlPort |= (1 << RegisterSelect);
	LCD_Allort(DisplayData & 0xF0); RollEnable();
	LCD_Allort((DisplayData << 4) & 0xF0); RollEnable();
}

void LCD_Disp(unsigned char DisplayAddress, const char *DisplayData)
{
	while(*DisplayData) LCD_Write(DisplayAddress++, *DisplayData++);
}

/*
void LCD_Decimal(unsigned char DisplayAddress, unsigned long int DecimalValue, char Length, const char Base)
{
	unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}*/

/*
void LCD_Float(unsigned char DisplayAddress, float Value, char PostFix, char PreFix, const char Seg)
{
	unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}
*/

void LCD_Allort(unsigned char Data)
{
	if(Data & (1 << 4)) LCDDataPort |= (1 << PinD4);
	else LCDDataPort &=~ (1 << PinD4);
	if(Data & (1 << 5)) LCDDataPort |= (1 << PinD5);
	else LCDDataPort &=~ (1 << PinD5);
	if(Data & (1 << 6)) LCDDataPort |= (1 << PinD6);
	else LCDDataPort &=~ (1 << PinD6);
	if(Data & (1 << 7)) LCDDataPort |= (1 << PinD7);
	else LCDDataPort &=~ (1 << PinD7);
}