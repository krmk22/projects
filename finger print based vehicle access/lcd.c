#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#include "lcd.h"

void LCD_Initialize()
{
	LCDDataDirection &=~ ((1 << PinD4) | (1 << PinD5) | (1 << PinD6) | (1 << PinD7));
	LCDControlDirection &=~ ((1 << RegisterSelect) | (1 << PinEnable));
	
	__delay_ms(100);
	LCD_Command(0x03);
	__delay_ms(4.5);
	LCD_Command(0x03);
	__delay_ms(4.5);
	LCD_Command(0x03);
	__delay_us(150);
	LCD_Command(0x02);
	__delay_us(150);
	LCD_Command(0x28);
	__delay_ms(4.5);
	LCD_Command(0x28);
	__delay_ms(4.5);
	LCD_Command(0x28);
	__delay_us(150);
	LCD_Command(0x06);
	__delay_us(150);
	LCD_Command(0x0C);
	__delay_us(150);
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

void LCD_Decimal(unsigned char DisplayAddress, unsigned long DecimalValue, char Length, const char Base)
{
	unsigned char LocalArray[10];
	dtoa(DecimalValue, (char*)LocalArray, Length, Base);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}

void LCD_Float(unsigned char DisplayAddress, float Value, char PostFix, char PreFix, const char Seg)
{
	unsigned char LocalArray[10];
	ftostra(Value, PostFix, PreFix, (char*)LocalArray, Seg);
	LCD_Disp(DisplayAddress, (char*)LocalArray);
}

void LCD_Allort(unsigned char Data)
{
	if(Data & 0x10) LCDDataPort |= (1 << PinD4);
	else LCDDataPort &=~ (1 << PinD4);
	if(Data & 0x20) LCDDataPort |= (1 << PinD5);
	else LCDDataPort &=~ (1 << PinD5);
	if(Data & 0x40) LCDDataPort |= (1 << PinD6);
	else LCDDataPort &=~ (1 << PinD6);
	if(Data & 0x80) LCDDataPort |= (1 << PinD7);
	else LCDDataPort &=~ (1 << PinD7);
}	

void LCD_LineClear(unsigned char Address, unsigned char Space)
{
	for(char k = 0; k < Space; k++)
	LCD_Write(Address + k, ' ');
}	 
