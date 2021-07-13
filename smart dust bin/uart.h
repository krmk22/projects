#ifndef _UART_H
#define _UART_H

#ifndef BaudCalculation
#define BaudCalculation
#define DesireBaudRate 9600
#define RegCalculation (((F_CPU / (DesireBaudRate * 16UL))) - 1)
#define CalculatedBaudRate (F_CPU / (16UL * (RegCalculation + 1)))
#define BaudRateError (CalculateBaudRate - DesireBaudRate)
#endif

#if BaudRateError > 200
#error "Selected Crystal End With Higher Communication Error"
#endif

void Serial_Initialize();
void Serial_Send(unsigned char);
void Serial_Write(const char*);
void Serial_Writeln(const char*, unsigned int);
void Serial_Decimal(unsigned long int, unsigned char, const char);
void Serial_Float(float, char, char, const char);
void Serial_Enable(const char);
unsigned char Serial_Receive();
#endif
