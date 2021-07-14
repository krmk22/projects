#include <htc.h>

#ifndef _GPIO_H
#include "gpio.h"
#endif

#ifndef _UTILS_H
#include "utils.h"
#endif

#include "spi.h"

char SerialClock;
char MasterOutSlaveIn;
char MasterInSlaveOut;

void SPI_Initialize(char SCK, char MOSI, char MISO)
{
	SerialClock = SCK;
	MasterOutSlaveIn = MOSI;
	MasterInSlaveOut = MISO;
	
	TRIS_OUTPUT(SerialClock);
	TRIS_OUTPUT(MasterOutSlaveIn);
	TRIS_INPUT(MasterInSlaveOut);
	
	PORT_HIGH(MasterOutSlaveIn);
	PORT_LOW(SerialClock);
}

void SPI_Write(unsigned char WriteData)
{
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(Shifting & WriteData) PORT_HIGH(MasterOutSlaveIn);
		else PORT_LOW(MasterOutSlaveIn);
		PORT_HIGH(SerialClock); PORT_LOW(SerialClock);
	}
}	

unsigned char SPI_Read()
{
	unsigned char ReturnVariable = 0x00;
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(PORT_READ(MasterInSlaveOut)) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting; 
		PORT_HIGH(SerialClock); PORT_LOW(SerialClock);
	}
	return ReturnVariable;
}

unsigned char SPI_RW(unsigned char WriteData)
{
	unsigned char ReturnVariable = 0x00;
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(Shifting & WriteData) PORT_HIGH(MasterOutSlaveIn);
		else PORT_LOW(MasterOutSlaveIn); PORT_HIGH(SerialClock);
		if(PORT_READ(MasterInSlaveOut)) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting; PORT_LOW(SerialClock);
	}
	return ReturnVariable;
}

void SPI_SWrite(unsigned char *WriteData, unsigned int Length)
{
	for(unsigned int k = 0; k < Length; k++)
	SPI_Write(WriteData[k]);
}

void SPI_SRead(unsigned char *ReadData, unsigned int Length)
{
	for(unsigned int k = 0; k < Length; k++)
	ReadData[k] = SPI_Read();
}

void SPI_SRW(unsigned char *WriteData, unsigned int Length)
{
	for(unsigned int k = 0; k < Length; k++)
	WriteData[k] = SPI_RW(WriteData[k]);
}	