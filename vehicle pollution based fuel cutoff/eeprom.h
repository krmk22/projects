#ifndef _EEPROM_H
#define _EEPROM_H

unsigned char eeprom_read(unsigned char);
void eeprom_write(unsigned char, unsigned char);
void eeprom_readln(unsigned char, unsigned char*, unsigned int);
void eeprom_writeln(unsigned char, const char*, unsigned int);
#endif
