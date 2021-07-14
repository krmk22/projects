#include <Arduino.h>
#include <avr/io.h>

#ifndef _UTILS_H
#include "utils.h"
#endif

#ifndef _I2C_H
#include "i2c.h"
#endif

#include "ds1307.h"

void ds1307_Write(unsigned char Address, unsigned char Data)
{
	I2C_Start();
	I2C_Write(DS1307Address);
	I2C_Write(Address);
	I2C_Write(Data);
	I2C_Stop();
}

unsigned char ds1307_Read(unsigned char Address)
{
	I2C_Start();
	I2C_Write(DS1307Address);
	I2C_Write(Address);
	I2C_Restart();
	I2C_Write(DS1307Address | 1);
	char ReturnVariable = I2C_Read(0);
	I2C_Stop();
	return ReturnVariable;
}

void UpdateTime(unsigned char Var1, unsigned char Var2, unsigned char Var3)
{
	ds1307_Write(RegHours, Dec2Hex(Var1));
	ds1307_Write(RegMinutes, Dec2Hex(Var2));
	ds1307_Write(RegSeconds, Dec2Hex(Var3));
}

void UpdateDate(unsigned char Var1, unsigned char Var2, unsigned char Var3, unsigned char Var4)
{
	ds1307_Write(RegDay, Dec2Hex(Var1));
	ds1307_Write(RegDate, Dec2Hex(Var2));
	ds1307_Write(RegMonth, Dec2Hex(Var3));
	ds1307_Write(RegYear, Dec2Hex(Var4));
}

void UpdateRTC(unsigned char Var1, unsigned char Var2, unsigned char Var3, unsigned char Var4, unsigned char Var5, unsigned char Var6, unsigned char Var7)
{
	UpdateTime(Var1, Var2, Var3);
	UpdateDate(Var4, Var5, Var6, Var7);
}

void FetchTime(unsigned char *Var1, unsigned char *Var2, unsigned char *Var3)
{
	*Var1 = Hex2Dec(ds1307_Read(RegHours));
	*Var2 = Hex2Dec(ds1307_Read(RegMinutes));
	*Var3 = Hex2Dec(ds1307_Read(RegSeconds));
}

void FetchDate(unsigned char *Var1, unsigned char *Var2, unsigned char *Var3, unsigned char *Var4)
{
	*Var1 = Hex2Dec(ds1307_Read(RegDay));
	*Var2 = Hex2Dec(ds1307_Read(RegDate));
	*Var3 = Hex2Dec(ds1307_Read(RegMonth));
	*Var4 = Hex2Dec(ds1307_Read(RegYear));
}

void FetchRTC(unsigned char *Var1, unsigned char *Var2, unsigned char *Var3, unsigned char *Var4, unsigned char *Var5, unsigned char *Var6, unsigned char *Var7)
{
	*Var1 = Hex2Dec(ds1307_Read(RegHours));
	*Var2 = Hex2Dec(ds1307_Read(RegMinutes));
	*Var3 = Hex2Dec(ds1307_Read(RegSeconds));
	*Var4 = Hex2Dec(ds1307_Read(RegDay));
	*Var5 = Hex2Dec(ds1307_Read(RegDate));
	*Var6 = Hex2Dec(ds1307_Read(RegMonth));
	*Var7 = Hex2Dec(ds1307_Read(RegYear));
}	
