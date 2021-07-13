#include <at89x52.h>
#include <intrins.h>

#ifndef _HEADER_H
#include "header.h"
#endif

#include "i2c.h"

void I2C_Initialize()
{
	I2CPort |= (1 << I2CData);
	I2CPort |= (1 << I2CClock);
}

void I2C_Start()
{
	I2CPort |= (1 << I2CClock);
	I2CPort |= (1 << I2CData);
	TunedDelay();
	I2CPort &=~ (1 << I2CData);
	TunedDelay();
	I2CPort &=~ (1 << I2CClock);
}

void I2C_Stop()
{
	I2CPort |= (1 << I2CClock);
	I2CPort &=~(1 << I2CData);
	TunedDelay();
	I2CPort |= (1 << I2CData);
	TunedDelay();
}


void I2C_Restart()
{
	I2CPort |= (1 << I2CData);
	TunedDelay();
	I2CPort |= (1 << I2CClock);
	TunedDelay();
	I2CPort &=~ (1 << I2CData);
	TunedDelay(); 
	I2CPort &=~ (1 << I2CClock);
	TunedDelay();
}


/*
void I2C_Restart()
{
	I2CPort &=~ (1 << I2CData);
	TunedDelay();
	I2CPort &=~ (1 << I2CClock);
	I2CPort |= (1 << I2CData);
	I2CPort |= (1 << I2CClock);
	TunedDelay();
	I2CPort &=~ (1 << I2CData);
	TundeDelay();
	I2CPort &=~ (1 << I2CClock);
}
*/

void I2C_Clock()
{
	_nop_();
	I2CPort |= (1 << I2CClock);
	TunedDelay();
	I2CPort &=~ (1 << I2CClock);
	TunedDelay();
	_nop_();
}

void I2C_Write(unsigned char Data)
{
	unsigned char Shifting;
	for(Shifting = 0x80; Shifting; Shifting >>= 1)
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
	unsigned char ReturnVariable, Shifting;
	for(Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(I2CLineIn) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting;
		I2C_Clock();
	}
	if(Ack) I2CPort &=~ (1 << I2CData);
	else I2CPort |= (1 << I2CData);
	I2C_Clock();
	return ReturnVariable;
}