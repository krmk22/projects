#include <pic.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

#ifndef _HEADER_H
#include "header.h"
#endif

#include "spi.h"

void SPI_Initialize()
{
	SPIDirection &=~ (1 << SPIClock);
	SPIDirection &=~ (1 << SPIMOSI);
	SPIDirection |= (1 << SPIMISO);
	
	MOSI = 0; SCK = 1; 
}

void SPI_Write(unsigned char WriteData)
{
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		MOSI = (Shifting & WriteData ?1 :0);
		SCK = 1; SCK = 0;
	}
}	

unsigned char SPI_Read()
{
	unsigned char ReturnVariable = 0x00;
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		if(MISO) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting;
		SCK = 1; SCK = 0;
	}
	return ReturnVariable;
}

unsigned char SPI_RW(unsigned char WriteData)
{
	unsigned char ReturnVariable = 0x00;
	for(unsigned char Shifting = 0x80; Shifting; Shifting >>= 1)
	{
		MOSI = (Shifting & WriteData ?1 :0); SCK = 1;
		if(MISO) ReturnVariable |= Shifting;
		else ReturnVariable &=~ Shifting; SCK = 0;
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
	
		
			
		
	