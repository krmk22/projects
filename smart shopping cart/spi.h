#ifndef _SPI_H
#define _SPI_H

void SPI_Initialize(char, char, char);
void SPI_Write(unsigned char);
unsigned char SPI_Read();
unsigned char SPI_RW(unsigned char);
#endif
