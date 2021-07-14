#ifndef _SUART_H
#define _SUART_H

#ifndef SerialConfig
#define SerialConfig
#define SerialPort	 PORTC
#define SerialDire	 TRISC
#define Transmitter	 0
#define Receiver 	 1
#endif

volatile bit TXR @ ((unsigned) &SerialPort * 8) + Transmitter;
volatile bit RXR @ ((unsigned) &SerialPort * 8) + Receiver;

extern unsigned char SSerialArray[100], SSerialCount;

extern void SSerial_Initialize(unsigned long);
extern void SSerial_Send(unsigned char);
extern void SSerial_Write(const char*);
extern void SSerial_Writeln(const char*, unsigned int);
extern unsigned char SSerial_Receive();
extern void SSerial_Decimal(unsigned long int, int, const);
extern void SSerial_Float(float, int, int, const);
extern void SSerial_Enable(int);
extern void SSerial_Flush();
extern char SSTalkBack(int, const char*, unsigned char, unsigned int);
#endif