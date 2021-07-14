#include <Arduino.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _I2C_H
#include "i2c.h"
#endif

#define LCDAddress 0x4E

#include "lcd.h"

char RegisterSelect;

void RollEnable()
{
  
}

void LCDClear()
{
  LCD_Command(0x01);
  delay(20);
}

void LCD_Initialize(char SCK, char SDA)
{
  I2C_Initialize(SCK, SDA);
  DelayMS(250);

  LCD_Command(0x03);
  _delay_ms(4.5);
  LCD_Command(0x03);
  _delay_ms(4.5);
  LCD_Command(0x03);
  _delay_us(150);
  LCD_Command(0x02);
  _delay_us(150);
  LCD_Command(0x28);
  _delay_ms(4.5);
  LCD_Command(0x28);
  _delay_ms(4.5);
  LCD_Command(0x28);
  _delay_us(150);
  LCD_Command(0x06);
  _delay_us(150);
  LCD_Command(0x0C);
  _delay_us(150);
  LCDClear();
}

void LCD_Command(unsigned char CommandData)
{
  RegisterSelect = 0;
  LCD_Allort(CommandData & 0xF0); RollEnable();
  LCD_Allort((CommandData << 4) & 0xF0); RollEnable();
}

void LCD_Write(unsigned char CommandData, unsigned char DisplayData)
{
  LCD_Command(CommandData);
  RegisterSelect = 1;
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
  I2C_Start();
  I2C_Write(LCDAddress);
  I2C_Write(Data | (0 << 2) | (1 << 3) | (RegisterSelect << 0));
  I2C_Write(Data | (1 << 2) | (1 << 3) | (RegisterSelect << 0));
  I2C_Write(Data | (0 << 2) | (1 << 3) | (RegisterSelect << 0));
  I2C_Stop();
}

void LCD_CustomChar(unsigned char Location, const char *DisplayData)
{
  Location *= 8;
  for(char k = 0; k < 8; k++) LCD_Write((0x40 + Location + k), DisplayData[k]);
}

void LCD_CustomDisp(unsigned char DisplayAddress, unsigned char T_Location, const char *DisplayData)
{
  LCD_CustomChar(T_Location, DisplayData); 
  LCD_Write(DisplayAddress, T_Location);
}
