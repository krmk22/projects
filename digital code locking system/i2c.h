#ifndef _I2C_H
#define _I2C_H

#ifndef I2CPinConfig
#define I2CPinConfig
#define I2CPort P2
#define I2CData 1
#define I2CClock 0
#endif

#define I2CLineIn (I2CPort & (1 << I2CData) ?1 :0)

void I2C_Initialize();
void I2C_Start();
void I2C_Stop();
void I2C_Restart();
void I2C_Clock();
void I2C_Write(unsigned char);
unsigned char I2C_Read(char);
#endif