#include <avr/io.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#include "i2c.h"

void I2C_Initialize()
{
	I2CDirection |= (1 << I2CData);
	I2CDirection |= (1 << I2CClock);	
}

void I2C_Start()
{
	I2CPort |= (1 << I2CClock);
	I2CPort |= (1 << I2CData);
	_delay_us(BusFreeTime);
	I2CPort &=~ (1 << I2CData);
	_delay_us(StartHoldTime);
	I2CPort &=~ (1 << I2CClock);
}

void I2C_Stop()
{
	I2CPort |= (1 << I2CClock);
	I2CPort &=~ (1 << I2CData);
	_delay_us(BusFreeTime);
	I2CPort |= (1 << I2CData);
	_delay_us(StopHoldTime);
}

void I2C_Restart()
{
	I2CPort |= (1 << I2CData);
	I2CPort |= (1 << I2CClock);
	_delay_us(BusFreeTime);
	I2CPort &=~ (1 << I2CData);
	_delay_us(RestartHoldTime);
	I2CPort &=~ (1 << I2CClock);
}

void I2C_Clock()
{
	_delay_us(DataHoldTime);
	I2CPort |= (1 << I2CClock);
	_delay_us(ClockHighTime);
	I2CPort &=~ (1 << I2CClock);
	_delay_us(ClockLowTime);
	_delay_us(DataSetupTime);
}

void I2C_Write(unsigned char Data)
{
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(Shifting & Data) I2CPort |= (1 << I2CData);
		else I2CPort &=~ (1 << I2CData);
		I2C_Clock();
	}
	I2CPort &=~ (1 << I2CData);
	I2C_Clock();
}

unsigned char I2C_Read(char Ack)
{
	unsigned char ReturnVariable = 0x00;
	I2CDirection &=~ (1 << I2CData);
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(I2CLineIn) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting;
		I2C_Clock();
	}
	I2CDirection |= (1 << I2CData);
	if(Ack) I2CPort &=~ (1 << I2CData);
	else I2CPort |= (1 << I2CData);
	I2C_Clock();
	return ReturnVariable;
}

