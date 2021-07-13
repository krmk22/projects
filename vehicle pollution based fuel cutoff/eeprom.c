#include <avr/io.h>
#include <avr/eeprom.h>

#ifndef _CLOCK_H
#include "clock.h"
#endif

void eeprom_write(unsigned char Address, unsigned char Data)
{
	eeprom_write_byte((unsigned char*)Address, (unsigned int)Data);
}

unsigned char eeprom_read(unsigned char Address)
{
	return eeprom_read_byte((unsigned char*)Address);
}

void eeprom_writeln(unsigned char Address, const char *Data, unsigned char Length)
{
	eeprom_write_block(Data, (unsigned char*)Address, (unsigned int)Length);
}

void eeprom_readln(unsigned char Address, unsigned char *StoreArray, unsigned char Length)
{
	eeprom_read_block(StoreArray, (unsigned char*)Address, (unsigned int)Length);
}
