#ifndef _I2C_H
#define _I2C_H

#define BusFreeTime   5
#define StartHoldTime 4
#define ClockLowTime  5
#define ClockHighTime 5
#define RestartHoldTime 5
#define DataHoldTime  1
#define DataSetupTime 1
#define StopHoldTime  5

extern void I2C_Start();
extern void I2C_Stop();
extern void I2C_Restart();
extern void I2C_Clock();

extern void I2C_Initialize(char, char);
extern void I2C_Write(unsigned char);
extern unsigned char I2C_Read(unsigned char);

void TWI_Write(const unsigned int, unsigned char, unsigned char);
unsigned char TWI_Read(const unsigned int, unsigned char);
void TWI_SeqWrite(const unsigned int, unsigned char, unsigned char*, unsigned char);
void TWI_SeqRead(const unsigned int, unsigned char, unsigned char*, unsigned char);

#endif
