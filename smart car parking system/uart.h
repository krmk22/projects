#ifndef _UART_H
#define _UART_H

extern unsigned char SerialArray[100], SerialCount;

extern void Serial_Initialize(unsigned long);
extern void Serial_Send(unsigned char);
extern void Serial_Write(const char*);
extern void Serial_Writeln(const char*, unsigned int);
extern unsigned char Serial_Receive();
extern void Serial_Decimal(unsigned int, unsigned int, const char);
extern void Serial_Float(float, char, char, const char);
extern void Serial_Enable(const char);
extern void Serial_Flush();
extern char STalkBack(const char, const char*, unsigned char, unsigned int);
#endif
