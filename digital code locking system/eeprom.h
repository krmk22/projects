#ifndef _EEPROM_H
#define _EEPROM_H

#ifndef MemoryDefinitions
#define MemoryDefinitions
#define NumberOfBitsKB 2048
#define NumberOfBytes NumberOfBits / 8
#define SinglePageByte 8
#endif

#ifndef EEPROMAddress
#define EEPROMAddress 0xA0
#endif

void EEPROM_Write(unsigned char, unsigned char);
unsigned char EEPROM_Read(unsigned char);
void EEPROM_Writeln(unsigned char, const char*);
void EEPROM_Readln(unsigned char, unsigned char*, char);
#endif