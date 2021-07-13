#ifndef _LCD_H
#define _LCD_H

#ifndef LCDDataConfig
#define LCDDataConfig
#define LCDDataPort PORTC
#define LCDDataDirection DDRC
#define LCDDataPin PINC
#define PinD4 2
#define PinD5 3
#define PinD6 4
#define PinD7 5
#endif

#ifndef LCDControlConfig
#define LCDControlConfig
#define LCDControlPort PORTC
#define LCDControlDirection DDRC
#define LCDControlPin PINC
#define RegisterSelect 0
#define PinEnable 1
#endif

#ifndef RollEnable
#define RollEnable()\
{\
	LCDControlPort &=~ (1 << PinEnable); _delay_us(5);\
	LCDControlPort |= (1 << PinEnable); _delay_us(5);\
	LCDControlPort &=~ (1 << PinEnable); _delay_us(100);\
}
#endif

#ifndef LCDClear
#define LCDClear()\
{\
	LCD_Command(0x01);\
	_delay_ms(50);\
}
#endif

void LCD_Initialize();
void LCD_Command(unsigned char);
void LCD_Write(unsigned char, unsigned char);
void LCD_Disp(unsigned char, const char*);
void LCD_Decimal(unsigned char, unsigned long, char, const char);
void LCD_Float(unsigned char, float, char, char, const char);
void LCD_Allort(unsigned char);
#endif
