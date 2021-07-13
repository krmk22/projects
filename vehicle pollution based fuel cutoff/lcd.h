#ifndef _LCD_H
#define _LCD_H

#ifndef LCDDataConfig
#define LCDDataConfig
#define LCDDataPort PORTD
#define LCDDataDirection DDRD
#define LCDDataPin PIND
#define PinD4 5
#define PinD5 4
#define PinD6 3
#define PinD7 2
#endif

#ifndef LCDControlConfig
#define LCDControlConfig
#define LCDControlPort PORTB
#define LCDControlDirection DDRB
#define LCDControlPin PINB
#define RegisterSelect 4
#define PinEnable 3
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
	_delay_ms(20);\
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
