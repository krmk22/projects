#ifndef _SUART_H
#define _SUART_H

/*
SUART is Program Controller Serial
Communication. This Library is Additional
Incase The Module Requires More Than One
Communication. Any GPIO Pin will act as
Transmitter and Receiver. But, With Out
Activating Interrupt Receiver Pin Will Miss
The Receive Data. While Transmitting Data All
Interrupts Are Temporarly UnAvailable Due to 
Timing Criticals. Here, This Library Used CCP1
Pin as Receiver Pin. While Using PWM Pulse 
Note That PulseWidthModulation1(); Not Used
*/

#ifndef DefaultTiming
#define DefaultTiming
#define CallDelay (8 * OneMachineCycle)
#define LoopDelay (18 * OneMachineCycle)
#define InterDelay (22 * OneMachineCycle) //23
#define BasicDelay (2 * OneMachineCycle)
#endif

#ifndef SoftBaudConfig
#define SoftBaudConfig
#define BaudRate 9600
#define BitPerUs 1000000UL / BaudRate
#endif

#ifndef TuningTime
#define TuningTime
#define FCallDelay (BitPerUs - CallDelay)
#define FLoopDelay (BitPerUs - LoopDelay)
#define FIntDelay (BitPerUs - InterDelay)
#define FStopDelay BitPerUs
#endif

#ifndef SerialConfig
#define SerialConfig
#define SerialPort PORTC
#define SerialDirection TRISC
#define Receiver 2
#define Transmitter 3
#endif

static bit SSerialReceive @ ((unsigned) &SerialPort * 8 + Receiver);

void SSerial_Initialize();
unsigned char SSerial_Receive();
void SSerial_Send(unsigned char);
void SSerial_Write(const char*);
void SSerial_Writeln(const char*, unsigned int);
void SSerial_Decimal(unsigned long int, unsigned char, const);
void SSerial_Float(float, unsigned char, unsigned char, const);
void SSerial_Enable(const);
#endif
