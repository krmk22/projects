#ifndef _LCD_H
#define _LCD_H

#ifndef LCDDataConfig
#define LCDDataConfig
#define LCDDataPort PORTB
#define LCDDataDirection TRISB
#define PinD4 3
#define PinD5 2
#define PinD6 1
#define PinD7 0
#endif

#ifndef LCDControlConfig
#define LCDControlConfig
#define LCDControlPort PORTB
#define LCDControlDirection TRISB
#define RegisterSelect 5
#define PinEnable 4
#endif

#ifndef RollEnable
#define RollEnable()\
{\
	LCDControlPort &=~ (1 << PinEnable); __delay_us(5);\
	LCDControlPort |= (1 << PinEnable); __delay_us(5);\
	LCDControlPort &=~ (1 << PinEnable); __delay_us(100);\
}
#endif

#ifndef LCDClear
#define LCDClear()\
{\
	LCD_Command(0x01);\
	__delay_ms(20);\
}
#endif

void LCD_Initialize();
void LCD_Command(unsigned char);
void LCD_Write(unsigned char, unsigned char);
void LCD_Disp(unsigned char, const char*);
void LCD_Decimal(unsigned char, unsigned long, char, const char);
void LCD_Float(unsigned char, float, char, char, const char);
void LCD_Allort(unsigned char);
void LCD_CustomChar(unsigned char, const char*);
void LCD_CustomDisp(unsigned char, unsigned char, const char*);
#endif
