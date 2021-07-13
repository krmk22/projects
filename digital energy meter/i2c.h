#ifndef _I2C_H
#define _I2C_H

#ifndef TimingCycles
#define TimingCycles
#define BusFreeTime 5
#define StartHoldTime 4
#define ClockLowTime 5
#define ClockHighTime 5
#define RestartHoldTime 5
#define DataHoldTime 1
#define DataSetupTime 1
#define StopHoldTime 5
#endif

#ifndef I2CConfig
#define I2CConfig
#define I2CPort PORTD
#define I2CDirection DDRD
#define I2CPin PIND
#define I2CData 6
#define I2CClock 5
#endif

#define I2CLineIn (I2CPin & (1 << I2CData) ?1 :0)

void I2C_Initialize();
void I2C_Start();
void I2C_Stop();
void I2C_Restart();
void I2C_Clock();
void I2C_Write(unsigned char);
unsigned char I2C_Read(char);
#endif
