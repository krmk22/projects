#include <at89x52.h>
#include <intrins.h>

#ifndef _HEADER_H
#include "header.h"
#endif

#ifndef _I2C_H
#include "i2c.h"
#endif

#include "eeprom.h"

void EEPROM_Write(unsigned char Address, unsigned char Data)
{
	I2C_Start();
	I2C_Write(EEPROMAddress);
	I2C_Write(Address);
	I2C_Write(Data);
	I2C_Stop();
}

unsigned char EEPROM_Read(unsigned char Address)
{
	unsigned char ReturnVariable;
	I2C_Start();
	I2C_Write(EEPROMAddress);
	I2C_Write(Address);
	I2C_Restart();
	I2C_Write(EEPROMAddress | 1);
	ReturnVariable = I2C_Read(0);
	I2C_Stop();
	return ReturnVariable;
}

void EEPROM_Writeln(unsigned char Address, const char *Data)
{
	while(*Data) EEPROM_Write(Address++, *Data++);
}

void EEPROM_Readln(unsigned char Address, unsigned char *StoreArray, char Length)
{
	unsigned char LocalVariable;
	for(LocalVariable = 0; LocalVariable < Length; LocalVariable++)
	StoreArray[LocalVariable] = EEPROM_Read(Address + LocalVariable);
}