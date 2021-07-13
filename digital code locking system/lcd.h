#ifndef _LCD_H
#define _LCD_H

#ifndef LCDDataConfig
#define LCDDataConfig
#define LCDDataPort P1
#define PinD4 4
#define PinD5 5
#define PinD6 6
#define PinD7 7
#endif

#ifndef LCDControlConfig
#define LCDControlConfig
#define LCDControlPort P3
#define RegisterSelect 5
#define PinEnable 4
#endif

#ifndef RollEnable
#define RollEnable()\
{\
	LCDControlPort &=~ (1 << PinEnable); TunedDelay();\
	LCDControlPort |=  (1 << PinEnable); TunedDelay();\
	LCDControlPort &=~ (1 << PinEnable); DelayUS(100);\
}
#endif

#ifndef LCDClear
#define LCDClear()\
{\
	LCD_Command(0x01);\
	DelayMS(20);\
}
#endif

void LCD_Initialize();
void LCD_Command(unsigned char);
void LCD_Write(unsigned char, unsigned char);
void LCD_Disp(unsigned char, const char*);
//void LCD_Decimal(unsigned char, unsigned long int, char, const char);
//void LCD_Float(unsigned char, float, char, char, const char);
void LCD_Allort(unsigned char);
#endif