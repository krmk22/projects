#ifndef _UART_H
#define _UART_H

/*
Below Formule is fetched From Official
DataSheet. If ErrorRate is More Than 
200 it'll be Resultant with huge chage
in BaudRate
*/

#ifndef BaudCalculation
#define BaudCalculation
#define DesireBaudRate 9600
#define RegCalculation (((_XTAL_FREQ / (DesireBaudRate * 16UL)))-1)
#define CalculatedBaudRate (_XTAL_FREQ / (16UL * (RegCalculation + 1)))
#define BaudRateError (CalculatedBaudRate - DesireBaudRate) 
#endif


#if BaudRateError > 200
#error "Selected Crystal End With Higher Error Rate"
#endif


void Serial_Initialize();
void Serial_Send(unsigned char);
void Serial_Write(const char*);
void Serial_Writeln(const char*, unsigned int);
void Serial_Decimal(unsigned long int, unsigned char, const);
void Serial_Float(float, unsigned char, unsigned char, const);
void Serial_Enable(const char);
#endif
